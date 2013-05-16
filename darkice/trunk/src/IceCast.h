/*------------------------------------------------------------------------------

   Copyright (c) 2000-2007 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : IceCast.h
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

        /**
         *  Mount point of the stream on the server.
         */
        char              * mountPoint;

        /**
         *  Remote dump file if any.
         */
        char              * remoteDumpFile;

        /**
         *  Description of the stream.
         */
        char              * description;

        /**
         *  Initalize the object.
         *
         *  @param mountPoint mount point of the stream on the server.
         *  @param remoteDumpFile remote dump file (may be NULL).
         *  @param description description of the stream.
         *  @exception Exception
         */
        void
        init (  const char            * mountPoint,
                const char            * description,
                const char            * remoteDumpFile )
                                                    throw ( Exception );

        /**
         *  De-initalize the object.
         *
         *  @exception Exception
         */
        void
        strip ( void )                              throw ( Exception );


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
         *  @param streamDump an optional sink to dump the binary stream
         *                    data to.
         *  @param bufferDuration duration of the BufferedSink buffer
         *                        in seconds.
         *  @exception Exception
         */
        inline
        IceCast (   TcpSocket         * socket,
                    const char        * password,
                    const char        * mountPoint,
                    unsigned int        bitRate,
                    const char        * name           = 0,
                    const char        * description    = 0,
                    const char        * url            = 0,
                    const char        * genre          = 0,
                    bool                isPublic       = false,
                    const char        * remoteDumpFile = 0,
                    Sink              * streamDump     = 0 )
                                                        throw ( Exception )
              : CastSink( socket,
                          password,
                          bitRate,
                          name,
                          url,
                          genre,
                          isPublic,
                          streamDump )
        {
            init( mountPoint, description, remoteDumpFile);
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
            init( cs.getMountPoint(),
                  cs.getDescription(),
                  cs.getRemoteDumpFile() );
        }

        /**
         *  Destructor.
         *
         *  @exception Exception
         */
        inline virtual
        ~IceCast( void )                           throw ( Exception )
        {
            strip();
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
                strip();
                CastSink::operator=( cs );
                init( cs.getMountPoint(),
                      cs.getDescription(),
                      cs.getRemoteDumpFile() );
            }
            return *this;
        }

        /**
         *  Get the mount point of the stream on the server.
         *
         *  @return the mount point of the stream on the server.
         */
        inline const char *
        getMountPoint ( void ) const                throw ()
        {
            return mountPoint;
        }

        /**
         *  Get the remote dump file if any.
         *
         *  @return the remote dump file. May be NULL.
         */
        inline const char *
        getRemoteDumpFile ( void ) const            throw ()
        {
            return remoteDumpFile;
        }

        /**
         *  Get the description of the stream.
         *
         *  @return the description of the stream.
         */
        inline const char *
        getDescription ( void ) const               throw ()
        {
            return description;
        }

};


/* ================================================= external data structures */


/* ====================================================== function prototypes */



#endif  /* ICE_CAST_H */

