/*------------------------------------------------------------------------------

   Copyright (c) 2000 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : CastSink.cpp
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

#include "Util.h"
#include "Exception.h"
#include "CastSink.h"


/* ===================================================  local data structures */


/* ================================================  local constants & macros */

/*------------------------------------------------------------------------------
 *  File identity
 *----------------------------------------------------------------------------*/
static const char fileid[] = "$Id$";


/* ===============================================  local function prototypes */


/* =============================================================  module code */

/*------------------------------------------------------------------------------
 *  Initialize the object
 *----------------------------------------------------------------------------*/
void
CastSink :: init (  TcpSocket             * socket,
                    Sink                  * streamDump,
                    const char            * password,
                    unsigned int            bitRate,
                    const char            * name,
                    const char            * url,
                    const char            * genre,
                    bool                    isPublic,
                    unsigned int            bufferDuration )
                                                        throw ( Exception )
{
    this->socket         = socket;
    this->streamDump     = streamDump;
    this->password       = password       ? Util::strDup( password) : 0;
    this->bitRate        = bitRate;
    this->name           = name           ? Util::strDup( name)     : 0;
    this->url            = url            ? Util::strDup( url)      : 0;
    this->genre          = genre          ? Util::strDup( genre)    : 0;
    this->isPublic       = isPublic;
    this->bufferDuration = bufferDuration;

    int       bufferSize = bitRate ? (bitRate * 1024 / 8) * bufferDuration
                                   : (128 * 1024 / 8) * bufferDuration;

    bufferedSink = socket ?  new BufferedSink( socket, bufferSize)
                          : 0;
}


/*------------------------------------------------------------------------------
 *  De-initialize the object
 *----------------------------------------------------------------------------*/
void
CastSink :: strip ( void )                          throw ( Exception )
{
    if ( isOpen() ) {
        close();
    }

    if ( password ) {
        delete[] password;
    }
    if ( name ) {
        delete[] name;
    }
    if ( url ) {
        delete[] url;
    }
    if ( genre ) {
        delete[] genre;
    }
}


/*------------------------------------------------------------------------------
 *  Open the connection
 *----------------------------------------------------------------------------*/
bool
CastSink :: open ( void )                       throw ( Exception )
{
    if ( isOpen() ) {
        return false;
    }

    if ( !bufferedSink->open() ) {
        return false;
    }

    if ( !sendLogin() ) {
        close();
        return false;
    }

    if ( streamDump != 0 ) {
        if ( !streamDump->isOpen() ) {
            if ( !streamDump->open() ) {
                reportEvent( 2, "can't open stream dump");
            }
        }
    }
    
    return true;
}



/*------------------------------------------------------------------------------
 
  $Source$

  $Log$
  Revision 1.9  2002/04/09 13:10:43  darkeye
  resolved memory leak issue introduced in 0.9

  Revision 1.8  2002/03/28 16:40:55  darkeye
  slight changes to allow for variable bitrate streams
  (where the value of bitrate is 0)

  Revision 1.7  2002/02/28 09:49:25  darkeye
  added possibility to save the encoded stream to a local file only
  (no streaming server needed)

  Revision 1.6  2002/02/20 11:54:11  darkeye
  added local dump file possibility

  Revision 1.5  2001/09/09 11:27:31  darkeye
  added support for ShoutCast servers

  Revision 1.4  2001/08/29 21:08:30  darkeye
  made some description options in the darkice config file optional

  Revision 1.3  2000/11/12 14:54:50  darkeye
  added kdoc-style documentation comments

  Revision 1.2  2000/11/10 20:14:11  darkeye
  added support for remote dump file

  Revision 1.1.1.1  2000/11/05 10:05:48  darkeye
  initial version

  
------------------------------------------------------------------------------*/

