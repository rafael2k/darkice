/*------------------------------------------------------------------------------

   Copyright (c) 2000-2007 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : IceCast2.h
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
#ifndef ICE_CAST2_H
#define ICE_CAST2_H

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
 *  Class representing output to an IceCast2 server with
 *  ice login
 *
 *  @author  $Author$
 *  @version $Revision$
 */
class IceCast2 : public CastSink
{
    public:

        /**
         *  Type for specifying the format of the stream.
         */
       enum StreamFormat { mp3, mp2, oggVorbis, oggOpus, oggFlac, aac, aacp };


    private:

        /**
         *  The format of the stream.
         */
        StreamFormat        format;

        /**
         *  Mount point of the stream on the server.
         */
        char              * mountPoint;

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
        init (  StreamFormat            format,
                const char            * mountPoint,
                const char            * description )
                                                    ;

        /**
         *  De-initalize the object.
         *
         *  @exception Exception
         */
        void
        strip ( void )                              ;


    protected:

        /**
         *  Default constructor. Always throws an Exception.
         *
         *  @exception Exception
         */
        inline
        IceCast2 ( void )                            
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
        sendLogin ( void )              ;


    public:

        /**
         *  Constructor.
         *
         *  @param socket socket connection to the server.
         *  @param username to be passed to the server
         *  @param password password to the server.
         *  @param mountPoint mount point of the stream on the server.
         *  @param format the format of the stream.
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
        IceCast2 (  TcpSocket         * socket,
                    const char        * username,
                    const char        * password,
                    const char        * mountPoint,
                    StreamFormat        format,
                    unsigned int        bitRate,
                    const char        * name           = 0,
                    const char        * description    = 0,
                    const char        * url            = 0,
                    const char        * genre          = 0,
                    bool                isPublic       = false,
                    Sink              * streamDump     = 0 )
                                                        
              : CastSink( socket,
                          username,
                          password,
                          bitRate,
                          name,
                          url,
                          genre,
                          isPublic,
                          streamDump )
        {
            init( format, mountPoint, description);
        }

        /**
         *  Copy constructor.
         *
         *  @param cs the IceCast2 to copy.
         */
        inline
        IceCast2(   const IceCast2 &    cs )        
                : CastSink( cs )
        {
            init( cs.getFormat(),
                  cs.getMountPoint(),
                  cs.getDescription() );
        }

        /**
         *  Destructor.
         *
         *  @exception Exception
         */
        inline virtual
        ~IceCast2( void )                           
        {
            strip();
        }

        /**
         *  Assignment operator.
         *
         *  @param cs the IceCast2 to assign this to.
         *  @return a reference to this IceCast2.
         *  @exception Exception
         */
        inline virtual IceCast2 &
        operator= ( const IceCast2 &    cs )        
        {
            if ( this != &cs ) {
                strip();
                CastSink::operator=( cs );
                init( cs.getFormat(),
                      cs.getMountPoint(),
                      cs.getDescription() );
            }
            return *this;
        }

        /**
         *  Get the format of the stream.
         *
         *  @return the format of the stream.
         */
        inline StreamFormat
        getFormat ( void ) const                    throw ()
        {
            return format;
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



#endif  /* ICE_CAST2_H */

