/*------------------------------------------------------------------------------

   Copyright (c) 2000 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : IceCast2.cpp
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
#include "IceCast2.h"


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
IceCast2 :: init (  const char            * mountPoint,
                    const char            * description )
                                                        throw ( Exception )
{
    this->mountPoint     = Util::strDup( mountPoint);
    this->description    = description    ? Util::strDup( description) : 0;
}


/*------------------------------------------------------------------------------
 *  De-initialize the object
 *----------------------------------------------------------------------------*/
void
IceCast2 :: strip ( void )                           throw ( Exception )
{
    delete[] mountPoint;
    if ( description ) {
        delete[] description;
    }
}


/*------------------------------------------------------------------------------
 *  Log in to the IceCast2 server
 *----------------------------------------------------------------------------*/
bool
IceCast2 :: sendLogin ( void )                           throw ( Exception )
{
    Sink          * sink   = getSink();
    Source        * source = getSocket();
    const char    * str;
    char            resp[STRBUF_SIZE];

    if ( !source->isOpen() ) {
        return false;
    }
    if ( !sink->isOpen() ) {
        return false;
    }

    /* send the request, a string like:
     * "SOURCE <mountpoint> ICE/1.0" */
    str = "SOURCE /";
    sink->write( str, strlen( str));
    str = getMountPoint();
    sink->write( str, strlen( str));
    str = " ICE/1.0";
    sink->write( str, strlen( str));

    /* send the content type, Ogg Vorbis */
    str = "\nContent-type: application/x-ogg";
    sink->write( str, strlen( str));

    /* send the ice- headers */
    str = "\nice-password: ";
    sink->write( str, strlen(str));
    str = getPassword();
    sink->write( str, strlen(str));

    str = "\nice-bitrate: ";
    sink->write( str, strlen( str));
    if ( snprintf( resp, STRBUF_SIZE, "%d", getBitRate()) == -1 ) {
        throw Exception( __FILE__, __LINE__, "snprintf overflow");
    }
    sink->write( resp, strlen( resp));

    str = "\nice-public: ";
    sink->write( str, strlen( str));
    str = getIsPublic() ? "1" : "0";
    sink->write( str, strlen( str));

    if ( getName() ) {
        str = "\nice-name: ";
        sink->write( str, strlen( str));
        str = getName();
        sink->write( str, strlen( str));
    }

    if ( getDescription() ) {
        str = "\nice-description: ";
        sink->write( str, strlen( str));
        str = getDescription();
        sink->write( str, strlen( str));
    }

    if ( getUrl() ) {
        str = "\nice-url: ";
        sink->write( str, strlen( str));
        str = getUrl();
        sink->write( str, strlen( str));
    }

    if ( getGenre() ) {
        str = "\nice-genre: ";
        sink->write( str, strlen( str));
        str = getGenre();
        sink->write( str, strlen( str));
    }

    str = "\n\n";
    sink->write( str, strlen( str));
    sink->flush();

    return true;
}



/*------------------------------------------------------------------------------
 
  $Source$

  $Log$
  Revision 1.3  2002/02/19 15:24:26  darkeye
  send Content-type header when logging in to icecast2 servers

  Revision 1.2  2001/11/20 09:06:18  darkeye
  fixed public stream reporting

  Revision 1.1  2001/09/14 19:31:06  darkeye
  added IceCast2 / vorbis support


  
------------------------------------------------------------------------------*/

