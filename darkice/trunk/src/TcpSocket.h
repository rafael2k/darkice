/*------------------------------------------------------------------------------

   Copyright (c) 2000-2007 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : TcpSocket.h
   Version  : $Revision: 474 $
   Author   : $Author: rafael@riseup.net $
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
#ifndef TCP_SOCKET_H
#define TCP_SOCKET_H

#ifndef __cplusplus
#error This is a C++ include file
#endif


/* ============================================================ include files */

#include "Source.h"
#include "Sink.h"
#include "Reporter.h"


/* ================================================================ constants */


/* =================================================================== macros */


/* =============================================================== data types */

/**
 *  A TCP network socket
 *
 *  @author  $Author: rafael@riseup.net $
 *  @version $Revision: 474 $
 */
class TcpSocket : public Source, public Sink, public virtual Reporter
{
    private:

        /**
         *  Name of the host this socket connects to.
         */
        char              * host;

        /**
         *  Port to connect to.
         */
        unsigned short      port;

        /**
         *  Low-level socket descriptor.
         */
        int                 sockfd;
        
        /**
         *  Initialize the object.
         *
         *  @param host name of the host this socket connects to.
         *  @param port port to connect to.
         *  @exception Exception
         */
        void
        init (  const char        * host,
                unsigned short      port )              throw ( Exception );

        /**
         *  De-initialize the object.
         *
         *  @exception Exception
         */
        void
        strip ( void )                                  throw ( Exception );


    protected:

        /**
         *  Default constructor. Always throws an Exception.
         *
         *  @exception Exception
         */
        inline
        TcpSocket ( void )                             throw ( Exception )
        {
            throw Exception( __FILE__, __LINE__);
        }


    public:

        /**
         *  Constructor.
         *
         *  @param host name of the host this socket connects to.
         *  @param port port to connect to.
         *  @exception Exception
         */
        inline
        TcpSocket(   const char        * host,
                     unsigned short      port )         throw ( Exception )
        {
            init( host, port);
        }

        /**
         *  Copy constructor.
         *
         *  @param ss the TcpSocket to copy.
         *  @exception Exception
         */
        TcpSocket(   const TcpSocket &    ss )        throw ( Exception );

        /**
         *  Destructor.
         *
         *  @exception Exception
         */
        inline virtual
        ~TcpSocket( void )                           throw ( Exception )
        {
            strip();
        }

        /**
         *  Assignment operator.
         *
         *  @param ss the TcpSocket to assign this to.
         *  @return a reference to this TcpSocket.
         *  @exception Exception
         */
        inline virtual TcpSocket &
        operator= ( const TcpSocket &    ss )        throw ( Exception );

        /**
         *  Get the host this socket connects to.
         *
         *  @return the host this socket connects to.
         */
        inline const char *
        getHost ( void ) const                      throw ()
        {
            return host;
        }

        /**
         *  Get the port this socket connects to.
         *
         *  @return the port this socket connects to.
         */
        inline unsigned int
        getPort ( void ) const                      throw ()
        {
            return port;
        }

        /**
         *  Open the TcpSocket.
         *
         *  @return true if opening was successfull, false otherwise.
         *  @exception Exception
         */
        virtual bool
        open ( void )                               throw ( Exception );

        /**
         *  Check if the TcpSocket is open.
         *
         *  @return true if the TcpSocket is open, false otherwise.
         */
        inline virtual bool
        isOpen ( void ) const                       throw ()
        {
            return sockfd != 0;
        }

        /**
         *  Check if the TcpScoket can be read from.
         *  Blocks until the specified time for data to be available.
         *
         *  @param sec the maximum seconds to block.
         *  @param usec micro seconds to block after the full seconds.
         *  @return true if the TcpSocket is ready to be read from,
         *          false otherwise.
         *  @exception Exception
         */
        virtual bool
        canRead (      unsigned int    sec,
                       unsigned int    usec )       throw ( Exception );

        /**
         *  Read from the TcpSocket.
         *
         *  @param buf the buffer to read into.
         *  @param len the number of bytes to read into buf
         *  @return the number of bytes read (may be less than len).
         *  @exception Exception
         */
        virtual unsigned int
        read (         void          * buf,
                       unsigned int    len )        throw ( Exception );


        /**
         *  Check if the TcpSocket is ready to accept data.
         *  Blocks until the specified time for data to be available.
         *
         *  @param sec the maximum seconds to block.
         *  @param usec micro seconds to block after the full seconds.
         *  @return true if the TcpSocket is ready to accept data,
         *          false otherwise.
         *  @exception Exception
         */
        virtual bool
        canWrite (     unsigned int    sec,
                       unsigned int    usec )       throw ( Exception );

        /**
         *  Write data to the TcpSocket.
         *
         *  @param buf the data to write.
         *  @param len number of bytes to write from buf.
         *  @return the number of bytes written (may be less than len).
         *  @exception Exception
         */
        virtual unsigned int
        write (        const void    * buf,
                       unsigned int    len )        throw ( Exception );

        /**
         *  Flush all data that was written to the TcpSocket to the underlying
         *  connection.
         *
         *  @exception Exception
         */
        inline virtual void
        flush ( void )                              throw ( Exception )
        {
        }

        /**
         *  Cut what the sink has been doing so far, and start anew.
         *  This usually means separating the data sent to the sink up
         *  until now, and start saving a new chunk of data.
         *
         *  For TcpSocket, this is a no-op.
         */
        inline virtual void
        cut ( void )                                    throw ()
        {
        }

        /**
         *  Close the TcpSocket.
         *
         *  @exception Exception
         */
        virtual void
        close ( void )                              throw ( Exception );
};


/* ================================================= external data structures */


/* ====================================================== function prototypes */



#endif  /* TCP_SOCKET_H */

