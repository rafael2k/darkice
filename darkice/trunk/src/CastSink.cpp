/*------------------------------------------------------------------------------

   Copyright (c) 2000 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : CastSink.cpp
   Version  : $Revision$
   Author   : $Author$
   Location : $Source$
   
   Abstract : 

     Data output to a ShoutCast / IceCast / etc. server

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
                    const char            * password,
                    const char            * mountPoint,
                    const char            * remoteDumpFile,
                    const char            * name,
                    const char            * description,
                    const char            * url,
                    const char            * genre,
                    unsigned int            bitRate,
                    bool                    isPublic,
                    unsigned int            bufferDuration )
                                                        throw ( Exception )
{
    this->socket         = socket;
    this->password       = Util::strDup( password);
    this->mountPoint     = Util::strDup( mountPoint);
    this->remoteDumpFile = remoteDumpFile ? Util::strDup( remoteDumpFile) : 0;
    this->name           = Util::strDup( name);
    this->description    = Util::strDup( description);
    this->url            = Util::strDup( url);
    this->genre          = Util::strDup( genre);
    this->bitRate        = bitRate;
    this->isPublic       = isPublic;
    this->bufferDuration = bufferDuration;

    bufferedSink = new BufferedSink( socket,
                                     (bitRate * 1024 / 8) * bufferDuration);

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

    delete[] password;
    delete[] mountPoint;
    if ( remoteDumpFile ) {
        delete[] remoteDumpFile;
    }
    delete[] name;
    delete[] description;
    delete[] url;
    delete[] genre;
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

    return true;
}



/*------------------------------------------------------------------------------
 
  $Source$

  $Log$
  Revision 1.2  2000/11/10 20:14:11  darkeye
  added support for remote dump file

  Revision 1.1.1.1  2000/11/05 10:05:48  darkeye
  initial version

  
------------------------------------------------------------------------------*/

