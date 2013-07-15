/*------------------------------------------------------------------------------

   Copyright (c) 2000-2007 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : ShoutCast.cpp
   Version  : $Revision$
   Author   : $Author$
   Location : $Source$
   
   Copyright notice:

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License  
    as published by the Free Software Foundation; either version 3
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

#ifdef HAVE_MATH_H
#include <math.h>
#else
#error need math.h
#endif

#include <sstream>


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
#define HEADERLINE_LENGTH     50

/* ===============================================  local function prototypes */


/* =============================================================  module code */

/*------------------------------------------------------------------------------
 *  Initialize the object
 *----------------------------------------------------------------------------*/
void
ShoutCast :: init ( const char            * irc,
                    const char            * aim,
                    const char            * icq,
                    const char            * mountPoint )
                                                        throw ( Exception )
{
    this->irc    = irc   ? Util::strDup( irc) : 0;
    this->aim    = aim   ? Util::strDup( aim) : 0;
    this->icq    = icq   ? Util::strDup( icq) : 0;
    this->mountPoint = mountPoint ? Util::strDup( mountPoint ) : 0;
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
    if (mountPoint ){
        delete[] mountPoint;
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
    bool            needsMountPoint = false;
    const char    * mountPoint      = getMountPoint();
    char            header_line[HEADERLINE_LENGTH+2]; // ... + \n + \0

    if ( !source->isOpen() ) {
        return false;
    }
    if ( !sink->isOpen() ) {
        return false;
    }

    // We will add SOURCE only if really needed: if the mountPoint is not null
    // and is different of "/". This is to keep maximum compatibility with
    // NullSoft Shoutcast server.
    if (mountPoint != 0L
     && strlen(mountPoint) > 0 && 0 != strcmp("/", mountPoint)) {
        needsMountPoint = true;
    }

    std::ostringstream os;

    if (needsMountPoint) {
        os << "SOURCE ";
    }

    /* first line is the password in itself */
    os << getPassword();
    os << "\n";
 
    // send the mount point 
    if (needsMountPoint) {
        os << " ";
        if (strncmp("/", mountPoint, 1) != 0) {
            os << "/";
        }
        os << mountPoint;
        os << "\n";
    }

    str = os.str().c_str();

    // Ok, now we send login which will be different of classical Shoutcast
    // if mountPoint is not null and is different from "/" ...
    sink->write( str, strlen( str));
    sink->flush();

    /* read the anticipated response: "OK" */
    len = source->read( resp, STRBUF_SIZE);
    reportEvent(8, "server response length: ", len);
    reportEvent(8, "server response: ", resp);

    if ( Util::strEq( resp, "invalid password",16) ) {
	throw Exception( __FILE__, __LINE__,
                         "ShoutCast - wrong password");
    }

    if ( len < 2 || resp[0] != 'O' || resp[1] != 'K' ) {
        return false;
    }

    /* suck anything that the other side has to say */
    while ( source->canRead( 0, 0) && 
           (len = source->read( resp, STRBUF_SIZE)) ) {
        ;
    }

    /* send the icy headers */
    if ( getName() ) {
      snprintf(header_line, HEADERLINE_LENGTH, "icy-name:%s", getName());
      strcat(header_line, "\n");
      sink->write( header_line, strlen( header_line));
    }

    if ( getUrl() ) {
      snprintf(header_line, HEADERLINE_LENGTH, "icy-url:%s", getUrl());
      strcat(header_line, "\n");
      sink->write( header_line, strlen( header_line));
    }

    if ( getGenre() ) {
      snprintf(header_line, HEADERLINE_LENGTH, "icy-genre:%s", getGenre());
      strcat(header_line, "\n");
      sink->write( header_line, strlen( header_line));
    }

    if ( getIrc() ) {
      snprintf(header_line, HEADERLINE_LENGTH, "icy-irc:%s", getIrc());
      strcat(header_line, "\n");
      sink->write( header_line, strlen( header_line));
    }

    if ( getAim() ) {
      snprintf(header_line, HEADERLINE_LENGTH, "icy-aim:%s", getAim());
      strcat(header_line, "\n");
      sink->write( header_line, strlen( header_line));
    }

    if ( getIcq() ) {
      snprintf(header_line, HEADERLINE_LENGTH, "icy-icq:%s", getIcq());
      strcat(header_line, "\n");
      sink->write( header_line, strlen( header_line));
    }

    snprintf(header_line, HEADERLINE_LENGTH, "icy-br:%d", getBitRate());
    strcat(header_line, "\n");
    sink->write( header_line, strlen( header_line));

    snprintf(header_line, HEADERLINE_LENGTH, "icy-pub:%s", getIsPublic() ? "1" : "0");
    strcat(header_line, "\n");
    sink->write( header_line, strlen( header_line));

    str = "\n";
    sink->write( str, strlen( str));
    sink->flush();

    return true;
}


