/*------------------------------------------------------------------------------

   Copyright (c) 2000 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : IceCast.h
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
#ifndef ICE_CAST_H
#define ICE_CAST_H

#ifndef __cplusplus
#error This is a C++ include file
#endif


/* ============================================================ include files */

#include "Sink.h"
#include "TcpSocket.h"
#include "CastSink.h"


/* ================================================================ constants */


/* =================================================================== macros */


/* =============================================================== data types */

/**
 *  Class representing output to an IceCast server with
 *  x-audiocast login
 *
 *  @author  $Author$
 *  @version $Revision$
 */
class IceCast : public CastSink
{
    private:


    protected:

        /**
         *  Default constructor. Always throws an Exception.
         *
         *  @exception Exception
         */
        inline
        IceCast ( void )                            throw ( Exception )
        {
            throw Exception( __FILE__, __LINE__);
        }

        /**
         *  Log in to the server using the socket avialable.
         *
         *  @return true if login was successful, false otherwise.
         *  @exception Exception
         */
        virtual bool
        sendLogin ( void )              throw ( Exception );


    public:

        /**
         *  Constructor.
         *
         *  @param socket socket connection to the server.
         *  @param password password to the server.
         *  @param mountPoint mount point of the stream on the server.
         *  @param remoteDumpFile remote dump file (may be NULL).
         *  @param name name of the stream.
         *  @param description description of the stream.
         *  @param url URL associated with the stream.
         *  @param genre genre of the stream.
         *  @param bitRate bitrate of the stream (e.g. mp3 bitrate).
         *  @param isPublic is the stream public?
         *  @param bufferDuration duration of the BufferedSink buffer
         *                        in seconds.
         *  @exception Exception
         */
        inline
        IceCast (   TcpSocket         * socket,
                    const char        * password,
                    const char        * mountPoint,
                    const char        * remoteDumpFile,
                    const char        * name,
                    const char        * description,
                    const char        * url,
                    const char        * genre,
                    unsigned int        bitRate,
                    bool                isPublic,
                    unsigned int        bufferDuration = 10 )
                                                        throw ( Exception )
              : CastSink( socket,
                          password,
                          mountPoint,
                          remoteDumpFile,
                          name,
                          description,
                          url,
                          genre,
                          bitRate,
                          isPublic,
                          bufferDuration )
        {
        }

        /**
         *  Copy constructor.
         *
         *  @param cs the IceCast to copy.
         */
        inline
        IceCast(   const IceCast &    cs )        throw ( Exception )
                : CastSink( cs )
        {
        }

        /**
         *  Destructor.
         *
         *  @exception Exception
         */
        inline virtual
        ~IceCast( void )                           throw ( Exception )
        {
        }

        /**
         *  Assignment operator.
         *
         *  @param cs the IceCast to assign this to.
         *  @return a reference to this IceCast.
         *  @exception Exception
         */
        inline virtual IceCast &
        operator= ( const IceCast &    cs )        throw ( Exception )
        {
            if ( this != &cs ) {
                CastSink::operator=( cs );
            }
            return *this;
        }
};


/* ================================================= external data structures */


/* ====================================================== function prototypes */



#endif  /* ICE_CAST_H */


/*------------------------------------------------------------------------------
 
  $Source$

  $Log$
  Revision 1.4  2000/11/12 14:54:50  darkeye
  added kdoc-style documentation comments

  Revision 1.3  2000/11/10 20:14:11  darkeye
  added support for remote dump file

  Revision 1.2  2000/11/05 17:37:24  darkeye
  removed clone() functions

  Revision 1.1.1.1  2000/11/05 10:05:52  darkeye
  initial version

  
------------------------------------------------------------------------------*/

