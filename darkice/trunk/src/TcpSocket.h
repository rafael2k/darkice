/*------------------------------------------------------------------------------

   Copyright (c) 2000 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : TcpSocket.h
   Version  : $Revision$
   Author   : $Author$
   Location : $Source$
   
   Abstract : 

     A TCP network socket

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
#ifndef TCP_SOCKET_H
#define TCP_SOCKET_H

#ifndef __cplusplus
#error This is a C++ include file
#endif


/* ============================================================ include files */

#include "Source.h"
#include "Sink.h"


/* ================================================================ constants */


/* =================================================================== macros */


/* =============================================================== data types */

/*------------------------------------------------------------------------------
 *  
 *----------------------------------------------------------------------------*/
class TcpSocket : public Source, public Sink
{
    private:

        char              * host;
        unsigned short      port;

        int                 sockfd;
        
        void
        init (  const char        * host,
                unsigned short      port )              throw ( Exception );


        void
        strip ( void )                                  throw ( Exception );


    protected:

        inline
        TcpSocket ( void )                             throw ( Exception )
        {
            throw Exception( __FILE__, __LINE__);
        }


    public:

        inline
        TcpSocket(   const char        * host,
                      unsigned short      port )        throw ( Exception )
        {
            init( host, port);
        }


        TcpSocket(   const TcpSocket &    ss )        throw ( Exception );


        inline virtual
        ~TcpSocket( void )                           throw ( Exception )
        {
            strip();
        }


        inline virtual TcpSocket &
        operator= ( const TcpSocket &    ss )        throw ( Exception );


        inline const char *
        getHost ( void ) const                      throw ()
        {
            return host;
        }


        inline unsigned int
        getPort ( void ) const                      throw ()
        {
            return port;
        }


        virtual bool
        open ( void )                               throw ( Exception );


        inline virtual bool
        isOpen ( void ) const                       throw ()
        {
            return sockfd != 0;
        }


        virtual bool
        canRead (      unsigned int    sec,
                       unsigned int    usec )       throw ( Exception );


        virtual unsigned int
        read (         void          * buf,
                       unsigned int    len )        throw ( Exception );


        virtual bool
        canWrite (     unsigned int    sec,
                       unsigned int    usec )       throw ( Exception );


        virtual unsigned int
        write (        const void    * buf,
                       unsigned int    len )        throw ( Exception );


        inline virtual void
        flush ( void )                              throw ( Exception )
        {
        }


        virtual void
        close ( void )                              throw ( Exception );
};


/* ================================================= external data structures */


/* ====================================================== function prototypes */



#endif  /* TCP_SOCKET_H */


/*------------------------------------------------------------------------------
 
  $Source$

  $Log$
  Revision 1.2  2000/11/05 17:37:24  darkeye
  removed clone() functions

  Revision 1.1.1.1  2000/11/05 10:05:55  darkeye
  initial version

  
------------------------------------------------------------------------------*/

