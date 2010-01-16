/*------------------------------------------------------------------------------

   Copyright (c) 2000-2007 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : RtpCast.h
   Version  : $Revision: 466 $
   Author   : $Author: piratfm $
   Location : $HeadURL$
   
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
#ifndef ICE_CAST2_H
#define ICE_CAST2_H

#ifndef __cplusplus
#error This is a C++ include file
#endif


/* ============================================================ include files */

#include "Ref.h"
#include "Reporter.h"
#include "Sink.h"
#include "NetSocket.h"
#include "BufferedSink.h"


/* ================================================================ constants */


/* =================================================================== macros */


/* =============================================================== data types */

/**
 *  Class representing output to an RtpCast server with
 *  ice login
 *
 *  @author  $Author: piratfm $
 *  @version $Revision: 466 $
 */
class RtpCast :  public Sink, public virtual Reporter
{
    public:

        /**
         *  Type for specifying the format of the stream.
         */
       enum StreamFormat { mp3, mp2, oggVorbis, aac, aacp };


    private:

        /**
         *  The format of the stream.
         */
        StreamFormat        format;

        /**
         *  The socket connection to the server.
         */
        Ref<NetSocket>      socket;

        /**
         *  The BufferedSink encapsulating the socket connection to the server.
         */
        Ref<BufferedSink>   bufferedSink;

        /**
         *  An optional Sink to enable stream dumps.
         */
        Ref<Sink>           streamDump;

        /**
         *  Duration of the BufferedSink buffer in seconds.
         */
        unsigned int        bufferDuration;


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
        RtpCast ( void )                            throw ( Exception )
        {
            throw Exception( __FILE__, __LINE__);
        }

        /**
         *  Get the Sink underneath this CastSink.
         *
         *  @return pointer to the Sink underneath this CastSink.
         */
        inline Sink *
        getSink ( void ) const                      throw ()
        {
            return bufferedSink.get();
        }

        /**
         *  Get the NetSocket underneath this CastSink.
         *
         *  @return pointer to the NetSocket underneath this CastSink.
         */
        inline NetSocket *
        getSocket ( void ) const                    throw ()
        {
            return socket.get();
        }


    public:

        /**
         *  Constructor.
         *
         *  @param socket socket connection to the server.
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
        RtpCast (  NetSocket         * socket,
                    const char        * password,
                    const char        * mountPoint,
                    StreamFormat        format,
                    unsigned int        bitRate,
                    const char        * name           = 0,
                    const char        * description    = 0,
                    const char        * url            = 0,
                    const char        * genre          = 0,
                    bool                isRtp       = false,
                    Sink              * streamDump     = 0,
                    unsigned int        bufferDuration = 10 )
                                                        throw ( Exception )
              : CastSink( socket,
                          password,
                          bitRate,
                          name,
                          url,
                          genre,
                          isPublic,
                          streamDump,
                          bufferDuration )
        {
            init( format, mountPoint, description);
        }

        /**
         *  Copy constructor.
         *
         *  @param cs the RtpCast to copy.
         */
        inline
        RtpCast(   const RtpCast &    cs )        throw ( Exception )
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
        ~RtpCast( void )                           throw ( Exception )
        {
            strip();
        }

        /**
         *  Assignment operator.
         *
         *  @param cs the RtpCast to assign this to.
         *  @return a reference to this RtpCast.
         *  @exception Exception
         */
        inline virtual RtpCast &
        operator= ( const RtpCast &    cs )        throw ( Exception )
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
         *  Open the CastSink.
         *  Logs in to the server.
         *
         *  @return true if opening was successfull, false otherwise.
         *  @exception Exception
         */
        virtual bool
        open ( void )                               throw ( Exception );

        /**
         *  Check if the CastSink is open.
         *
         *  @return true if the CastSink is open, false otherwise.
         */
        inline virtual bool
        isOpen ( void ) const                       throw ()
        {
            return bufferedSink != NULL ? bufferedSink->isOpen() : false;
        }

        /**
         *  Check if the CastSink is ready to accept data.
         *  Blocks until the specified time for data to be available.
         *
         *  @param sec the maximum seconds to block.
         *  @param usec micro seconds to block after the full seconds.
         *  @return true if the CastSink is ready to accept data,
         *          false otherwise.
         *  @exception Exception
         */
        inline virtual bool
        canWrite (     unsigned int    sec,
                       unsigned int    usec )       throw ( Exception )
        {
            return getSink()->canWrite( sec, usec);
        }

        /**
         *  Write data to the CastSink.
         *
         *  @param buf the data to write.
         *  @param len number of bytes to write from buf.
         *  @return the number of bytes written (may be less than len).
         *  @exception Exception
         */
        inline virtual unsigned int
        write (        const void    * buf,
                       unsigned int    len )        throw ( Exception )
        {
            if ( streamDump != 0 ) {
                streamDump->write( buf, len);
            }

            return getSink()->write( buf, len);
        }











        /**
         *  Flush all data that was written to the CastSink to the server.
         *
         *  @exception Exception
         */
        inline virtual void
        flush ( void )                              throw ( Exception )
        {
            if ( streamDump != 0 ) {
                streamDump->flush();
            }

            return getSink()->flush();
        }

        /**
         *  Cut what the sink has been doing so far, and start anew.
         *  This usually means separating the data sent to the sink up
         *  until now, and start saving a new chunk of data.
         */
        inline virtual void
        cut ( void )                                    throw ()
        {
            if ( streamDump != 0 ) {
                streamDump->cut();
            }
        }



        /**
         *  Close the CastSink.
         *
         *  @exception Exception
         */
        inline virtual void
        close ( void )                              throw ( Exception )
        {
            if ( streamDump != 0 ) {
                streamDump->close();
            }

            return getSink()->close();
        }


        /**
         *  Get the bitrate of the stream (e.g. mp3 bitrate).
         *
         *  @return the bitrate of the stream (e.g. mp3 bitrate).
         */
        inline unsigned int
        getBitRate ( void ) const                   throw ()
        {
            return bitRate;
        }

        /**
         *  Get the duration of the BufferedSink buffer in seconds.
         *
         *  @return the the duration of the BufferedSink buffer in seconds.
         */
        inline unsigned int
        getBufferDuration ( void ) const            throw ()
        {
            return bufferDuration;
        }

};


/* ================================================= external data structures */


/* ====================================================== function prototypes */



#endif  /* ICE_CAST2_H */

