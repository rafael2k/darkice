/*------------------------------------------------------------------------------

   Copyright (c) 2000-2007 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : ShoutCast.h
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
#ifndef SHOUT_CAST_H
#define SHOUT_CAST_H

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
 *  Class representing output to a ShoutCast server with
 *  icy login
 *
 *  @author  $Author$
 *  @version $Revision$
 */
class ShoutCast : public CastSink
{
    private:

        /**
         *  IRC info string for the stream
         */
        char              * irc;

        /**
         *  AIM info string for the stream
         */
        char              * aim;

        /**
         *  ICQ info string for the stream
         */
        char              * icq;

        /**
         * The optional mountPoint
         */
        char              * mountPoint;

        /**
         *  Initalize the object.
         *
         *  @param irc IRC info string for the stream.
         *  @param aim AIM info string for the stream.
         *  @param icq ICQ info string for the stream.
         *  @param mountPoint Optional mount point information
         *  @exception Exception
         */
        void
        init (  const char            * irc,
                const char            * aim,
                const char            * icq,
                const char            * mountPoint )
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
        ShoutCast ( void )                            throw ( Exception )
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
         *  @param mountPoint Optional mount point for DSS.
         *  @param name name of the stream.
         *  @param url URL associated with the stream.
         *  @param genre genre of the stream.
         *  @param bitRate bitrate of the stream (e.g. mp3 bitrate).
         *  @param isPublic is the stream public?
         *  @param irc IRC info string for the stream.
         *  @param aim AIM info string for the stream.
         *  @param icq ICQ info string for the stream.
         *  @param streamDump an optional sink to dump the binary stream
         *                    data to.
         *  @param bufferDuration duration of the BufferedSink buffer
         *                        in seconds.
         *  @exception Exception
         */
        inline
        ShoutCast ( TcpSocket         * socket,
                    const char        * password,
                    const char        * mountPoint,
                    unsigned int        bitRate,
                    const char        * name           = 0,
                    const char        * url            = 0,
                    const char        * genre          = 0,
                    bool                isPublic       = false,
                    const char        * irc            = 0,
                    const char        * aim            = 0,
                    const char        * icq            = 0,
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
            init( irc, aim, icq, mountPoint );
        }

        /**
         *  Copy constructor.
         *
         *  @param cs the ShoutCast to copy.
         */
        inline
        ShoutCast(   const ShoutCast &    cs )        throw ( Exception )
                : CastSink( cs )
        {
            init( cs.getIrc(), cs.getAim(), cs.getIcq(), cs.getMountPoint());
        }

        /**
         *  Destructor.
         *
         *  @exception Exception
         */
        inline virtual
        ~ShoutCast( void )                           throw ( Exception )
        {
            strip();
        }

        /**
         *  Assignment operator.
         *
         *  @param cs the ShoutCast to assign this to.
         *  @return a reference to this ShoutCast.
         *  @exception Exception
         */
        inline virtual ShoutCast &
        operator= ( const ShoutCast &    cs )        throw ( Exception )
        {
            if ( this != &cs ) {
                strip();
                CastSink::operator=( cs );
                init( cs.getIrc(), cs.getAim(), cs.getIcq(), getMountPoint());
            }
            return *this;
        }

       /**
         *  Get the mount point of the stream on the server.
         *  The mount point can be null if it has not been set
         *  (typical Shoutcast server) or not null (for instance
         *  with Darwin Streaming Server). In that case, the
         *  authentication process will be slightly different.
         *
         *  @return the mount point of the stream on the server.
         */
        inline const char *
        getMountPoint ( void ) const                throw ()
        {
            return mountPoint;
        }

        /**
         *  Get the IRC info string for the stream.
         *
         *  @return the IRC info string for the stream.
         */
        inline const char *
        getIrc ( void ) const                   throw ()
        {
            return irc;
        }

        /**
         *  Get the AIM info string for the stream.
         *
         *  @return the AIM info string for the stream.
         */
        inline const char *
        getAim ( void ) const                   throw ()
        {
            return aim;
        }

        /**
         *  Get the ICQ info string for the stream.
         *
         *  @return the ICQ info string for the stream.
         */
        inline const char *
        getIcq ( void ) const                   throw ()
        {
            return icq;
        }

};


/* ================================================= external data structures */


/* ====================================================== function prototypes */



#endif  /* SHOUT_CAST_H */

