/*------------------------------------------------------------------------------

   Copyright (c) 2000 Tyrell Corporation.
   Copyright (c) 2006 Clyde Stubbs.

   Tyrell DarkIce

   File     : SerialUlaw.cpp
   Version  : $Revision: 1.13 $
   Author   : $Author: darkeye $
   Location : $Source: /cvsroot/darkice/darkice/src/SerialUlaw.cpp,v $
   
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

/* ============================================================ include files */

#include "SerialUlaw.h"

#ifdef SUPPORT_SERIAL_ULAW
// only compile this code if there's support for it


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#else
#error need unistd.h
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

#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#else
#error need sys/stat.h
#endif

#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#else
#error need fcntl.h
#endif

#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#else
#error need sys/time.h
#endif

#ifdef HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#else
#error need sys/ioctl.h
#endif

#ifdef HAVE_TERMIOS_H
#include <termios.h>
#else
#error need termios.h
#endif


#include "Util.h"
#include "Exception.h"
#include "SerialUlaw.h"


/* ===================================================  local data structures */


/* ================================================  local constants & macros */

/*------------------------------------------------------------------------------
 *  File identity
 *----------------------------------------------------------------------------*/
static const char fileid[] = "$Id$";


/*------------------------------------------------------------------------------
 *  Ulaw decode table
 *----------------------------------------------------------------------------*/
static unsigned int ulawdecode[256] =
{
    0x8284,0x8684,0x8a84,0x8e84,0x9284,0x9684,0x9a84,0x9e84,
    0xa284,0xa684,0xaa84,0xae84,0xb284,0xb684,0xba84,0xbe84,
    0xc184,0xc384,0xc584,0xc784,0xc984,0xcb84,0xcd84,0xcf84,
    0xd184,0xd384,0xd584,0xd784,0xd984,0xdb84,0xdd84,0xdf84,
    0xe104,0xe204,0xe304,0xe404,0xe504,0xe604,0xe704,0xe804,
    0xe904,0xea04,0xeb04,0xec04,0xed04,0xee04,0xef04,0xf004,
    0xf0c4,0xf144,0xf1c4,0xf244,0xf2c4,0xf344,0xf3c4,0xf444,
    0xf4c4,0xf544,0xf5c4,0xf644,0xf6c4,0xf744,0xf7c4,0xf844,
    0xf8a4,0xf8e4,0xf924,0xf964,0xf9a4,0xf9e4,0xfa24,0xfa64,
    0xfaa4,0xfae4,0xfb24,0xfb64,0xfba4,0xfbe4,0xfc24,0xfc64,
    0xfc94,0xfcb4,0xfcd4,0xfcf4,0xfd14,0xfd34,0xfd54,0xfd74,
    0xfd94,0xfdb4,0xfdd4,0xfdf4,0xfe14,0xfe34,0xfe54,0xfe74,
    0xfe8c,0xfe9c,0xfeac,0xfebc,0xfecc,0xfedc,0xfeec,0xfefc,
    0xff0c,0xff1c,0xff2c,0xff3c,0xff4c,0xff5c,0xff6c,0xff7c,
    0xff88,0xff90,0xff98,0xffa0,0xffa8,0xffb0,0xffb8,0xffc0,
    0xffc8,0xffd0,0xffd8,0xffe0,0xffe8,0xfff0,0xfff8,0x0000,
    0x7d7c,0x797c,0x757c,0x717c,0x6d7c,0x697c,0x657c,0x617c,
    0x5d7c,0x597c,0x557c,0x517c,0x4d7c,0x497c,0x457c,0x417c,
    0x3e7c,0x3c7c,0x3a7c,0x387c,0x367c,0x347c,0x327c,0x307c,
    0x2e7c,0x2c7c,0x2a7c,0x287c,0x267c,0x247c,0x227c,0x207c,
    0x1efc,0x1dfc,0x1cfc,0x1bfc,0x1afc,0x19fc,0x18fc,0x17fc,
    0x16fc,0x15fc,0x14fc,0x13fc,0x12fc,0x11fc,0x10fc,0x0ffc,
    0x0f3c,0x0ebc,0x0e3c,0x0dbc,0x0d3c,0x0cbc,0x0c3c,0x0bbc,
    0x0b3c,0x0abc,0x0a3c,0x09bc,0x093c,0x08bc,0x083c,0x07bc,
    0x075c,0x071c,0x06dc,0x069c,0x065c,0x061c,0x05dc,0x059c,
    0x055c,0x051c,0x04dc,0x049c,0x045c,0x041c,0x03dc,0x039c,
    0x036c,0x034c,0x032c,0x030c,0x02ec,0x02cc,0x02ac,0x028c,
    0x026c,0x024c,0x022c,0x020c,0x01ec,0x01cc,0x01ac,0x018c,
    0x0174,0x0164,0x0154,0x0144,0x0134,0x0124,0x0114,0x0104,
    0x00f4,0x00e4,0x00d4,0x00c4,0x00b4,0x00a4,0x0094,0x0084,
    0x0078,0x0070,0x0068,0x0060,0x0058,0x0050,0x0048,0x0040,
    0x0038,0x0030,0x0028,0x0020,0x0018,0x0010,0x0008,0x0000,
};


/* ===============================================  local function prototypes */


/* =============================================================  module code */

/*------------------------------------------------------------------------------
 *  Tell if source id big endian
 *----------------------------------------------------------------------------*/
bool
SerialUlaw :: isBigEndian ( void ) const                  throw ()
{
    return false;
}


/*------------------------------------------------------------------------------
 *  Initialize the object
 *----------------------------------------------------------------------------*/
void
SerialUlaw :: init (  const char      * name )    throw ( Exception )
{
    fileName       = Util::strDup( name);
    fileDescriptor = 0;
    running        = false;
}


/*------------------------------------------------------------------------------
 *  De-initialize the object
 *----------------------------------------------------------------------------*/
void
SerialUlaw :: strip ( void )                      throw ( Exception )
{
    if ( isOpen() ) {
        close();
    }
    
    delete[] fileName;
}


/*------------------------------------------------------------------------------
 *  Open the audio source
 *----------------------------------------------------------------------------*/
bool
SerialUlaw :: open ( void )                       throw ( Exception )
{
    struct termios    ts;

    if ( isOpen() ) {
        return false;
    }

    switch ( getBitsPerSample() ) {
        case 16:
            break;

        default:
            return false;
    }

    if (getChannel() != 1) {
        reportEvent(3, "Only mono input supported for Serial ULaw");
        return false;
    }
    if (getSampleRate() != 8000) {
        reportEvent(3, "Only 8000 Hz sample rate supported for Serial ULaw");
        return false;
    }

    if ( (fileDescriptor = ::open( fileName, O_RDONLY)) == -1 ) {
        fileDescriptor = 0;
        return false;
    }

    if(tcgetattr(fileDescriptor, &ts) < 0) {
        close();
        throw Exception( __FILE__, __LINE__, "can't get tty settings");
    }

    cfsetispeed(&ts, B115200);
    cfmakeraw(&ts);
    ts.c_cflag |= CLOCAL;
    if(tcsetattr(fileDescriptor, TCSANOW, &ts) < 0) {
        close();
        throw Exception( __FILE__, __LINE__, "can't set tty settings");
    }

    tcflush(fileDescriptor, TCIFLUSH);

    return true;
}


/*------------------------------------------------------------------------------
 *  Check whether read() would return anything
 *----------------------------------------------------------------------------*/
bool
SerialUlaw :: canRead ( unsigned int    sec,
                          unsigned int    usec )    throw ( Exception )
{
    fd_set              fdset;
    struct timeval      tv;
    int                 ret;

    if ( !isOpen() ) {
        return false;
    }

    FD_ZERO( &fdset);
    FD_SET( fileDescriptor, &fdset);
    tv.tv_sec  = sec;
    tv.tv_usec = usec;

    ret = select( fileDescriptor + 1, &fdset, NULL, NULL, &tv);
    
    if ( ret == -1 ) {
        throw Exception( __FILE__, __LINE__, "select error");
    }

    return ret > 0;
}


/*------------------------------------------------------------------------------
 *  Read from the audio source
 *----------------------------------------------------------------------------*/
unsigned int
SerialUlaw :: read (    void          * buf,
                          unsigned int    len )     throw ( Exception )
{
    ssize_t         ret;
    unsigned char    ubuf[256], * ptr;
    int                i, plen;

    if ( !isOpen() ) {
        return 0;
    }

    ret = 0;
    ptr = (unsigned char *)buf;
    while(len > 1) {
        plen = sizeof(ubuf);
        if (plen > (int)len/2) {
            plen = len/2;
        }
        plen = ::read( fileDescriptor, ubuf, plen);
        if(plen < 0) {
            perror("read");
            throw Exception( __FILE__, __LINE__, "read error");
        }
        for(i = 0 ; i != plen ; i++) {
            *ptr++ = ulawdecode[ubuf[i]] & 0xFF;
            *ptr++ = ulawdecode[ubuf[i]] >> 8;
        }
        len -= plen*2;
        ret += plen*2;
    }

    running = true;
    return ret;
}


/*------------------------------------------------------------------------------
 *  Close the audio source
 *----------------------------------------------------------------------------*/
void
SerialUlaw :: close ( void )                  throw ( Exception )
{
    if ( !isOpen() ) {
        return;
    }

    ::close( fileDescriptor);
    fileDescriptor = 0;
    running        = false;
}


#endif // SUPPORT_SERIAL_ULAW
