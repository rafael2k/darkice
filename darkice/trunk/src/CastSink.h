/*------------------------------------------------------------------------------

   Copyright (c) 2000 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : CastSink.h
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
#ifndef CAST_SINK_H
#define CAST_SINK_H

#ifndef __cplusplus
#error This is a C++ include file
#endif


/* ============================================================ include files */

#include "Ref.h"
#include "Sink.h"
#include "TcpSocket.h"
#include "BufferedSink.h"


/* ================================================================ constants */


/* =================================================================== macros */


/* =============================================================== data types */

/*------------------------------------------------------------------------------
 *  
 *----------------------------------------------------------------------------*/
class CastSink : public Sink
{
    private:

        Ref<TcpSocket>      socket;
        Ref<BufferedSink>   bufferedSink;
        char              * password;
        char              * mountPoint;
        char              * remoteDumpFile;

        char              * name;
        char              * description;
        char              * url;
        char              * genre;
        unsigned int        bitRate;
        bool                isPublic;
        unsigned int        bufferDuration;


        void
        init (  TcpSocket             * socket,
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
                                                    throw ( Exception );


        void
        strip ( void )                              throw ( Exception );


    protected:

        inline
        CastSink ( void )                           throw ( Exception )
        {
            throw Exception( __FILE__, __LINE__);
        }


        virtual bool
        sendLogin ( void )              throw ( Exception )         = 0;


    public:

        inline
        CastSink (  TcpSocket         * socket,
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
        {
            init( socket,
                  password,
                  mountPoint,
                  remoteDumpFile,
                  name,
                  description,
                  url,
                  genre,
                  bitRate,
                  isPublic,
                  bufferDuration );
        }


        inline
        CastSink(   const CastSink &    cs )        throw ( Exception )
                : Sink( cs )
        {
            init( cs.socket.get(),
                  cs.password,
                  cs.mountPoint,
                  cs.remoteDumpFile,
                  cs.name,
                  cs.description,
                  cs.url,
                  cs.genre,
                  cs.bitRate,
                  cs.isPublic,
                  cs.bufferDuration );
        }


        inline virtual
        ~CastSink( void )                           throw ( Exception )
        {
            strip();
        }


        inline virtual CastSink &
        operator= ( const CastSink &    cs )        throw ( Exception )
        {
            if ( this != &cs ) {
                strip();
                Sink::operator=( cs );
                init( cs.socket.get(),
                      cs.password,
                      cs.mountPoint,
                      cs.remoteDumpFile,
                      cs.name,
                      cs.description,
                      cs.url,
                      cs.genre,
                      cs.bitRate,
                      cs.isPublic,
                      cs.bufferDuration );
            }
            return *this;
        }


        inline Sink *
        getSink ( void ) const                      throw ()
        {
            return bufferedSink.get();
        }


        inline TcpSocket *
        getSocket ( void ) const                    throw ()
        {
            return socket.get();
        }


        virtual bool
        open ( void )                               throw ( Exception );


        inline virtual bool
        isOpen ( void ) const                       throw ()
        {
            return bufferedSink->isOpen();
        }


        inline virtual bool
        canWrite (     unsigned int    sec,
                       unsigned int    usec )       throw ( Exception )
        {
            return bufferedSink->canWrite( sec, usec);
        }


        inline virtual unsigned int
        write (        const void    * buf,
                       unsigned int    len )        throw ( Exception )
        {
            return bufferedSink->write( buf, len);
        }


        inline virtual void
        flush ( void )                              throw ( Exception )
        {
            return bufferedSink->flush();
        }


        inline virtual void
        close ( void )                              throw ( Exception )
        {
            return bufferedSink->close();
        }

        
        inline const char *
        getPassword ( void ) const                  throw ()
        {
            return password;
        }


        inline const char *
        getMountPoint ( void ) const                throw ()
        {
            return mountPoint;
        }


        inline const char *
        getRemoteDumpFile ( void ) const            throw ()
        {
            return remoteDumpFile;
        }


        inline const char *
        getName ( void ) const                      throw ()
        {
            return name;
        }


        inline const char *
        getDescription ( void ) const               throw ()
        {
            return description;
        }


        inline const char *
        getUrl ( void ) const                       throw ()
        {
            return url;
        }


        inline const char *
        getGenre ( void ) const                     throw ()
        {
            return genre;
        }


        inline unsigned int
        getBitRate ( void ) const                   throw ()
        {
            return bitRate;
        }


        inline bool
        getIsPublic ( void ) const                  throw ()
        {
            return isPublic;
        }


        inline unsigned int
        getBufferDuration ( void ) const            throw ()
        {
            return bufferDuration;
        }
};


/* ================================================= external data structures */


/* ====================================================== function prototypes */



#endif  /* CAST_SINK_H */


/*------------------------------------------------------------------------------
 
  $Source$

  $Log$
  Revision 1.3  2000/11/10 20:14:11  darkeye
  added support for remote dump file

  Revision 1.2  2000/11/05 17:37:24  darkeye
  removed clone() functions

  Revision 1.1.1.1  2000/11/05 10:05:48  darkeye
  initial version

  
------------------------------------------------------------------------------*/

