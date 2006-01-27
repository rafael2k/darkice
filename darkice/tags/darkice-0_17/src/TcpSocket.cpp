/*------------------------------------------------------------------------------

   Copyright (c) 2000 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : TcpSocket.cpp
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

/* ============================================================ include files */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#else
#error need string.h
#endif

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#else
#error need sys/types.h
#endif

#ifdef HAVE_ERRNO_H
#include <errno.h>
#else
#error need errno.h
#endif

#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#else
#error need sys/socket.h
#endif

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#else
#error need netinet/in.h
#endif

#ifdef HAVE_NETDB_H
#include <netdb.h>
#else
#error need netdb.h
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#else
#error need unistd.h
#endif

#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#else
#error need sys/time.h
#endif


#include "Util.h"
#include "Exception.h"
#include "TcpSocket.h"


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
TcpSocket :: init (   const char    * host,
                      unsigned short  port )          throw ( Exception )
{
    this->host   = Util::strDup( host);
    this->port   = port;
    this->sockfd = 0;
}


/*------------------------------------------------------------------------------
 *  De-initialize the object
 *----------------------------------------------------------------------------*/
void
TcpSocket :: strip ( void)                           throw ( Exception )
{
    if ( isOpen() ) {
        close();
    }

    delete[] host;
}


/*------------------------------------------------------------------------------
 *  Copy Constructor
 *----------------------------------------------------------------------------*/
TcpSocket :: TcpSocket (  const TcpSocket &    ss )    throw ( Exception )
                : Source( ss), Sink( ss )
{
    int     fd;
    
    init( ss.host, ss.port);

    if ( (fd = ss.sockfd ? dup( ss.sockfd) : 0) == -1 ) {
        strip();
        throw Exception( __FILE__, __LINE__, "dup failure");
    }

    sockfd = fd;
}


/*------------------------------------------------------------------------------
 *  Assignment operator
 *----------------------------------------------------------------------------*/
TcpSocket &
TcpSocket :: operator= (  const TcpSocket &    ss )   throw ( Exception )
{
    if ( this != &ss ) {
        int     fd;

        /* first strip */
        strip();


        /* then build up */
        Sink::operator=( ss );
        Source::operator=( ss );

        init( ss.host, ss.port);
        
        if ( (fd = ss.sockfd ? dup( ss.sockfd) : 0) == -1 ) {
            strip();
            throw Exception( __FILE__, __LINE__, "dup failure");
        }

        sockfd = fd;
    }

    return *this;
}


/*------------------------------------------------------------------------------
 *  Open the file
 *----------------------------------------------------------------------------*/
bool
TcpSocket :: open ( void )                       throw ( Exception )
{
#ifdef HAVE_ADDRINFO
    struct addrinfo         hints
    struct addrinfo       * ptr;
    struct sockaddr_storage addr;
    char                    portstr[6];
#else
    struct sockaddr_in      addr;
    struct hostent        * pHostEntry;
#endif
 
    if ( isOpen() ) {
        return false;
    }

#ifdef HAVE_ADDRINFO
    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = AF_ANY;
    snprintf(portstr, sizeof(portstr), "%d", port);

    if (getaddrinfo(host , portstr, &hints, &ptr))
	throw Exception( __FILE__, __LINE__, "getaddrinfo error", errno);
    memcpy ( addr, ptr->ai_addr, ptr->ai_addrlen);
    freeaddrinfo(ptr);
#else
    if ( !(pHostEntry = gethostbyname( host)) ) {
        throw Exception( __FILE__, __LINE__, "gethostbyname error", errno);
    }
    
    if ( (sockfd = socket( AF_INET, SOCK_STREAM,  IPPROTO_TCP)) == -1 ) {
        throw Exception( __FILE__, __LINE__, "socket error", errno);
    }

    memset( &addr, 0, sizeof(addr));
    addr.sin_family      = AF_INET;
    addr.sin_port        = htons(port);
    addr.sin_addr.s_addr = *((long*) pHostEntry->h_addr_list[0]);
#endif
    if ( connect( sockfd, (struct sockaddr*)&addr, sizeof(addr)) == -1 ) {
        ::close( sockfd);
        sockfd = 0;
        throw Exception( __FILE__, __LINE__, "connect error", errno);
    }

    return true;
}


/*------------------------------------------------------------------------------
 *  Check wether read() would return anything
 *----------------------------------------------------------------------------*/
bool
TcpSocket :: canRead (      unsigned int    sec,
                            unsigned int    usec )      throw ( Exception )
{
    fd_set              fdset;
    struct timeval      tv;
    int                 ret;

    if ( !isOpen() ) {
        return false;
    }

    FD_ZERO( &fdset);
    FD_SET( sockfd, &fdset);
    tv.tv_sec  = sec;
    tv.tv_usec = usec;

    ret = select( sockfd + 1, &fdset, NULL, NULL, &tv);
    
    if ( ret == -1 ) {
        throw Exception( __FILE__, __LINE__, "select error");
    }

    return ret > 0;
}


/*------------------------------------------------------------------------------
 *  Read from the socket
 *----------------------------------------------------------------------------*/
unsigned int
TcpSocket :: read (     void          * buf,
                        unsigned int    len )       throw ( Exception )
{
    int         ret;

    if ( !isOpen() ) {
        return 0;
    }

    ret = recv( sockfd, buf, len, 0);

    if ( ret == -1 ) {
        throw Exception( __FILE__, __LINE__, "recv error", errno);
    }

    return ret;
}


/*------------------------------------------------------------------------------
 *  Check wether read() would return anything
 *----------------------------------------------------------------------------*/
bool
TcpSocket :: canWrite (    unsigned int    sec,
                           unsigned int    usec )      throw ( Exception )
{
    fd_set              fdset;
    struct timeval      tv;
    int                 ret;

    if ( !isOpen() ) {
        return false;
    }

    FD_ZERO( &fdset);
    FD_SET( sockfd, &fdset);
    tv.tv_sec  = sec;
    tv.tv_usec = usec;

    ret = select( sockfd + 1, NULL, &fdset, NULL, &tv);
    
    if ( ret == -1 ) {
        throw Exception( __FILE__, __LINE__, "select error");
    }

    return ret > 0;
}


/*------------------------------------------------------------------------------
 *  Write to the socket
 *----------------------------------------------------------------------------*/
unsigned int
TcpSocket :: write (    const void    * buf,
                        unsigned int    len )       throw ( Exception )
{
    int         ret;

    if ( !isOpen() ) {
        return 0;
    }

#ifdef HAVE_MSG_NOSIGNAL
    ret = send( sockfd, buf, len, MSG_NOSIGNAL);
#else
    ret = send( sockfd, buf, len, 0);
#endif

    if ( ret == -1 ) {
        if ( errno == EAGAIN ) {
            ret = 0;
        } else {
            throw Exception( __FILE__, __LINE__, "send error", errno);
        }
    }

    return ret;
}


/*------------------------------------------------------------------------------
 *  Close the socket
 *----------------------------------------------------------------------------*/
void
TcpSocket :: close ( void )                          throw ( Exception )
{
    if ( !isOpen() ) {
        return;
    }

    flush();
    ::close( sockfd);
    sockfd = 0;
}



/*------------------------------------------------------------------------------
 
  $Source$

  $Log$
  Revision 1.10  2005/04/11 19:34:23  darkeye
  added IPv6 support, thanks to <jochen2@users.sourceforge.net>

  Revision 1.9  2002/10/19 12:22:27  darkeye
  cosmetic change

  Revision 1.8  2002/08/28 18:24:46  darkeye
  ported to FreeBSD (removed reference to MSG_NOSIGNAL in TcpSocket.cpp)

  Revision 1.7  2002/07/20 16:37:06  darkeye
  added fault tolerance in case a server connection is dropped

  Revision 1.6  2001/09/18 16:44:10  darkeye
  TcpSocket did not report closed state when could not connect()

  Revision 1.5  2001/08/30 17:25:56  darkeye
  renamed configure.h to config.h

  Revision 1.4  2000/11/17 15:50:48  darkeye
  added -Wall flag to compiler and eleminated new warnings

  Revision 1.3  2000/11/12 14:54:50  darkeye
  added kdoc-style documentation comments

  Revision 1.2  2000/11/05 14:08:28  darkeye
  changed builting to an automake / autoconf environment

  Revision 1.1.1.1  2000/11/05 10:05:55  darkeye
  initial version

  
------------------------------------------------------------------------------*/

