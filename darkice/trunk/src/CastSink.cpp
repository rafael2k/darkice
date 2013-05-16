/*------------------------------------------------------------------------------

   Copyright (c) 2000-2007 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : CastSink.cpp
   Version  : $Revision$
   Author   : $Author$
   Location : $HeadURL$
   
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
                    bool                    isPublic )
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

    if ( !getSink()->open() ) {
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


