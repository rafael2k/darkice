/*------------------------------------------------------------------------------

   Copyright (c) 2000-2007 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : CastSink.h
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
#ifndef CAST_SINK_H
#define CAST_SINK_H

#ifndef __cplusplus
#error This is a C++ include file
#endif


/* ============================================================ include files */

#include "Ref.h"
#include "Reporter.h"
#include "Sink.h"
#include "TcpSocket.h"
#include "BufferedSink.h"


/* ================================================================ constants */


/* =================================================================== macros */


/* =============================================================== data types */

/**
 *  Data output to a ShoutCast / IceCast / etc. server
 *  This is an abstract class. A subclass should override at least
 *  the sendLogin() function.
 *
 *  @author  $Author$
 *  @version $Revision$
 */
class CastSink : public Sink, public virtual Reporter
{
    private:

        /**
         *  The socket connection to the server.
         */
        Ref<TcpSocket>      socket;

        /**
         *  An optional Sink to enable stream dumps.
         */
        Ref<Sink>           streamDump;

        /**
         *  Password to the server.
         */
        char              * password;

        /**
         *  Name of the stream.
         */
        char              * name;

        /**
         *  URL associated with the stream.
         */
        char              * url;

        /**
         *  Genre of the stream.
         */
        char              * genre;

        /**
         *  Bitrate of the stream (e.g. mp3 bitrate).
         */
        unsigned int        bitRate;

        /**
         *  Is the stream public?
         */
        bool                isPublic;

        /**
         *  Initalize the object.
         *
         *  @param socket socket connection to the server.
         *  @param password password to the server.
         *  @param name name of the stream.
         *  @param url URL associated with the stream.
         *  @param genre genre of the stream.
         *  @param bitRate bitrate of the stream (e.g. mp3 bitrate).
         *  @param isPublic is the stream public?
         *  @exception Exception
         */
        void
        init (  TcpSocket             * socket,
                Sink                  * streamDump,
                const char            * password,
                unsigned int            bitRate,
                const char            * name,
                const char            * url,
                const char            * genre,
                bool                    isPublic)
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
        CastSink ( void )                           throw ( Exception )
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
        sendLogin ( void )              throw ( Exception )         = 0;

        /**
         *  Get the Sink underneath this CastSink.
         *
         *  @return pointer to the Sink underneath this CastSink.
         */
        inline Sink *
        getSink ( void ) const                      throw ()
        {
            return getSocket();
        }

        /**
         *  Get the TcpSocket underneath this CastSink.
         *
         *  @return pointer to the TcpSocket underneath this CastSink.
         */
        inline TcpSocket *
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
         *  @param name name of the stream.
         *  @param url URL associated with the stream.
         *  @param genre genre of the stream.
         *  @param bitRate bitrate of the stream (e.g. mp3 bitrate).
         *  @param isPublic is the stream public?
         *  @param streamDump a Sink to dump the streamed binary data to
         *
         *  @exception Exception
         */
        inline
        CastSink (  TcpSocket         * socket,
                    const char        * password,
                    unsigned int        bitRate,
                    const char        * name           = 0,
                    const char        * url            = 0,
                    const char        * genre          = 0,
                    bool                isPublic       = false,
                    Sink              * streamDump     = 0)
                                                        throw ( Exception )
        {
            init( socket,
                  streamDump,
                  password,
                  bitRate,
                  name,
                  url,
                  genre,
                  isPublic );
        }

        /**
         *  Copy constructor.
         *
         *  @param cs the CastSink to copy.
         */
        inline
        CastSink(   const CastSink &    cs )        throw ( Exception )
                : Sink( cs )
        {
            init( cs.socket.get(),
                  cs.streamDump.get(),
                  cs.password,
                  cs.bitRate,
                  cs.name,
                  cs.url,
                  cs.genre,
                  cs.isPublic );
        }

        /**
         *  Destructor.
         *
         *  @exception Exception
         */
        inline virtual
        ~CastSink( void )                           throw ( Exception )
        {
            strip();
        }

        /**
         *  Assignment operator.
         *
         *  @param cs the CastSink to assign this to.
         *  @return a reference to this CastSink.
         *  @exception Exception
         */
        inline virtual CastSink &
        operator= ( const CastSink &    cs )        throw ( Exception )
        {
            if ( this != &cs ) {
                strip();
                Sink::operator=( cs );
                init( cs.socket.get(),
                      cs.streamDump.get(),
                      cs.password,
                      cs.bitRate,
                      cs.name,
                      cs.url,
                      cs.genre,
                      cs.isPublic );
            }
            return *this;
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
	    if( !getSink() ) {
		return false;
	    }
	    return getSink()->isOpen();
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
         *  Get the password to the server.
         *
         *  @return the password to the server.
         */
        inline const char *
        getPassword ( void ) const                  throw ()
        {
            return password;
        }

        /**
         *  Get the name of the stream.
         *
         *  @return the name of the stream.
         */
        inline const char *
        getName ( void ) const                      throw ()
        {
            return name;
        }

        /**
         *  Get the URL associated with the stream.
         *
         *  @return the URL associated with the stream.
         */
        inline const char *
        getUrl ( void ) const                       throw ()
        {
            return url;
        }

        /**
         *  Get the genre of the stream.
         *
         *  @return the genre of the stream.
         */
        inline const char *
        getGenre ( void ) const                     throw ()
        {
            return genre;
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
         *  Get whether this stream is public.
         *
         *  @return true if the stream is public, false otherwise.
         */
        inline bool
        getIsPublic ( void ) const                  throw ()
        {
            return isPublic;
        }
};


/* ================================================= external data structures */


/* ====================================================== function prototypes */



#endif  /* CAST_SINK_H */

