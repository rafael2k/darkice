/*------------------------------------------------------------------------------

   Copyright (c) 2000 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : IceCast.cpp
   Version  : $Revision$
   Author   : $Author$
   Location : $Source$
   
   Abstract : 

     Class representing output to an IceCast server with
     x-audiocast login

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
     Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307,
     USA.

------------------------------------------------------------------------------*/

/* ============================================================ include files */

#ifdef HAVE_CONFIG_H
#include "configure.h"
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
#include "IceCast.h"


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
bool
IceCast :: sendLogin ( void )                           throw ( Exception )
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

    /* send the request, a string like:
     * "SOURCE <password> /<mountpoint>\n\n" */
    str = "SOURCE ";
    sink->write( str, strlen( str));
    str = getPassword();
    sink->write( str, strlen( str));
    str = " /";
    sink->write( str, strlen( str));
    str = getMountPoint();
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

    /* send the x-audiocast headers */
    str = "x-audiocast-name: ";
    sink->write( str, strlen( str));
    str = getName();
    sink->write( str, strlen( str));

    str = "\nx-audiocast-description: ";
    sink->write( str, strlen( str));
    str = getDescription();
    sink->write( str, strlen( str));

    str = "\nx-audiocast-url: ";
    sink->write( str, strlen( str));
    str = getUrl();
    sink->write( str, strlen( str));

    str = "\nx-audiocast-genre: ";
    sink->write( str, strlen( str));
    str = getGenre();
    sink->write( str, strlen( str));

    str = "\nx-audiocast-bitrate: ";
    sink->write( str, strlen( str));
    if ( snprintf( resp, STRBUF_SIZE, "%d", getBitRate()) == -1 ) {
        throw Exception( __FILE__, __LINE__, "snprintf overflow");
    }
    sink->write( resp, strlen( resp));

    str = "\nx-audiocast-public: ";
    sink->write( str, strlen( str));
    str = getIsPublic() ? "yes" : "no";
    sink->write( str, strlen( str));

    if ( getRemoteDumpFile() ) {
        str = "\nx-audiocast-dumpfile: ";
        sink->write( str, strlen( str));
        str = getRemoteDumpFile();
        sink->write( str, strlen( str));
    }

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
  Revision 1.3  2000/11/10 20:14:11  darkeye
  added support for remote dump file

  Revision 1.2  2000/11/05 14:08:28  darkeye
  changed builting to an automake / autoconf environment

  Revision 1.1.1.1  2000/11/05 10:05:52  darkeye
  initial version

  
------------------------------------------------------------------------------*/

