/*------------------------------------------------------------------------------

   Copyright (c) 2000 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : ShoutCast.cpp
   Version  : $Revision$
   Author   : $Author$
   Location : $Source$
   
   Copyright notice:

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License  
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.
   
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of 
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
    GNU General Public License for more details.
   
    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

------------------------------------------------------------------------------*/

/* ============================================================ include files */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_STDIO_H
#include <stdio.h>
#else
#error need stdio.h
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#else
#error need string.h
#endif


#include "Exception.h"
#include "Source.h"
#include "Sink.h"
#include "Util.h"
#include "ShoutCast.h"


/* ===================================================  local data structures */


/* ================================================  local constants & macros */

/*------------------------------------------------------------------------------
 *  File identity
 *----------------------------------------------------------------------------*/
static const char fileid[] = "$Id$";


/*------------------------------------------------------------------------------
 *  Size of string conversion buffer
 *----------------------------------------------------------------------------*/
#define STRBUF_SIZE         32


/* ===============================================  local function prototypes */


/* =============================================================  module code */

/*------------------------------------------------------------------------------
 *  Initialize the object
 *----------------------------------------------------------------------------*/
void
ShoutCast :: init ( const char            * irc,
                    const char            * aim,
                    const char            * icq )
                                                        throw ( Exception )
{
    this->irc    = irc   ? Util::strDup( irc) : 0;
    this->aim    = aim   ? Util::strDup( aim) : 0;
    this->icq    = icq   ? Util::strDup( icq) : 0;
}


/*------------------------------------------------------------------------------
 *  De-initialize the object
 *----------------------------------------------------------------------------*/
void
ShoutCast :: strip ( void )                             throw ( Exception )
{
    if ( irc ) {
        delete[] irc;
    }
    if ( aim ) {
        delete[] aim;
    }
    if ( icq ) {
        delete[] icq;
    }
}


/*------------------------------------------------------------------------------
 *  Log in to the ShoutCast server using the icy login scheme
 *----------------------------------------------------------------------------*/
bool
ShoutCast :: sendLogin ( void )                           throw ( Exception )
{
    Sink          * sink   = getSink();
    Source        * source = getSocket();
    const char    * str;
    char            resp[STRBUF_SIZE];
    unsigned int    len;

    if ( !source->isOpen() ) {
        return false;
    }
    if ( !sink->isOpen() ) {
        return false;
    }

    /* first line is the password in itself */
    str = getPassword();
    sink->write( str, strlen( str));
    str = "\n";
    sink->write( str, strlen( str));
    sink->flush();

    /* send the icy headers */
    if ( getName() ) {
        str = "icy-name:";
        sink->write( str, strlen( str));
        str = getName();
        sink->write( str, strlen( str));
    }

    if ( getUrl() ) {
        str = "\nicy-url:";
        sink->write( str, strlen( str));
        str = getUrl();
        sink->write( str, strlen( str));
    }

    if ( getGenre() ) {
        str = "\nicy-genre:";
        sink->write( str, strlen( str));
        str = getGenre();
        sink->write( str, strlen( str));
    }

    if ( getIrc() ) {
        str = "\nicy-irc:";
        sink->write( str, strlen( str));
        str = getIrc();
        sink->write( str, strlen( str));
    }

    if ( getAim() ) {
        str = "\nicy-aim:";
        sink->write( str, strlen( str));
        str = getAim();
        sink->write( str, strlen( str));
    }

    if ( getIcq() ) {
        str = "\nicy-icq:";
        sink->write( str, strlen( str));
        str = getIcq();
        sink->write( str, strlen( str));
    }

    str = "\nicy-br:";
    sink->write( str, strlen( str));
    if ( snprintf( resp, STRBUF_SIZE, "%d", getBitRate()) == -1 ) {
        throw Exception( __FILE__, __LINE__, "snprintf overflow");
    }
    sink->write( resp, strlen( resp));

    str = "\nicy-pub:";
    sink->write( str, strlen( str));
    str = getIsPublic() ? "yes" : "no";
    sink->write( str, strlen( str));

    str = "\n\n";
    sink->write( str, strlen( str));
    sink->flush();

    /* read the anticipated response: "OK" */
    len = source->read( resp, STRBUF_SIZE);
    if ( len < 2 || resp[0] != 'O' || resp[1] != 'K' ) {
        return false;
    }

    /* suck anything that the other side has to say */
    while ( source->canRead( 0, 0) && 
           (len = source->read( resp, STRBUF_SIZE)) ) {
        ;
    }

    return true;
}



/*------------------------------------------------------------------------------
 
  $Source$

  $Log$
  Revision 1.1  2001/09/09 11:27:31  darkeye
  added support for ShoutCast servers


  
------------------------------------------------------------------------------*/

