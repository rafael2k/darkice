/*------------------------------------------------------------------------------

   Copyright (c) 2000 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : OssDspSource.cpp
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

#include "OssDspSource.h"

#ifdef SUPPORT_OSS_DSP
// only compile this code if there is support for it

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

#ifdef HAVE_SYS_SOUNDCARD_H
#include <sys/soundcard.h>
#else
#error need sys/soundcard.h
#endif


#include "Util.h"
#include "Exception.h"
#include "OssDspSource.h"


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
OssDspSource :: init (  const char      * name )    throw ( Exception )
{
    fileName       = Util::strDup( name);
    fileDescriptor = 0;
    running        = false;
}


/*------------------------------------------------------------------------------
 *  De-initialize the object
 *----------------------------------------------------------------------------*/
void
OssDspSource :: strip ( void )                      throw ( Exception )
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
OssDspSource :: open ( void )                       throw ( Exception )
{
    int             format;
    int             i;
    unsigned int    u;

    if ( isOpen() ) {
        return false;
    }

    switch ( getBitsPerSample() ) {
        case 8:
            format = AFMT_U8;
            break;

        case 16:
            format = AFMT_S16_LE;
            break;
            
        default:
            return false;
    }

    if ( (fileDescriptor = ::open( fileName, O_RDONLY)) == -1 ) {
        fileDescriptor = 0;
        return false;
    }

    i = format;
    if ( ioctl( fileDescriptor, SNDCTL_DSP_SETFMT, &i) == -1 ||
         i != format ) {
        
        close();
        throw Exception( __FILE__, __LINE__, "can't set format", i);
    }

    u = getChannel();
    if ( ioctl( fileDescriptor, SNDCTL_DSP_CHANNELS, &u) == -1 ||
         u != getChannel() ) {
        
        close();
        throw Exception( __FILE__, __LINE__, "can't set channels", u);
    }

    u = getSampleRate();
    if ( ioctl( fileDescriptor, SNDCTL_DSP_SPEED, &u) == -1 ) {

        close();
        throw Exception( __FILE__, __LINE__,
                         "can't set soundcard recording sample rate", u);
    }
    if ( u != getSampleRate() ) {
        reportEvent( 2, "sound card recording sample rate set to ", u,
                        " while trying to set it to ", getSampleRate());
        reportEvent( 2, "this is probably not a problem, but a slight "
                        "drift in the sound card driver");
    }

    return true;
}


/*------------------------------------------------------------------------------
 *  Check wether read() would return anything
 *----------------------------------------------------------------------------*/
bool
OssDspSource :: canRead ( unsigned int    sec,
                          unsigned int    usec )    throw ( Exception )
{
    fd_set              fdset;
    struct timeval      tv;
    int                 ret;

    if ( !isOpen() ) {
        return false;
    }

    if ( !running ) {
        /* ugly workaround to get the dsp into recording state */
        unsigned char * b =
                          new unsigned char[getChannel()*getBitsPerSample()/8];
        read( b, getChannel()*getBitsPerSample()/8);
        delete[] b;
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
OssDspSource :: read (    void          * buf,
                          unsigned int    len )     throw ( Exception )
{
    ssize_t     ret;

    if ( !isOpen() ) {
        return 0;
    }

    ret = ::read( fileDescriptor, buf, len);

    if ( ret == -1 ) {
        throw Exception( __FILE__, __LINE__, "read error");
    }

    running = true;
    return ret;
}


/*------------------------------------------------------------------------------
 *  Close the audio source
 *----------------------------------------------------------------------------*/
void
OssDspSource :: close ( void )                  throw ( Exception )
{
    if ( !isOpen() ) {
        return;
    }

    ::close( fileDescriptor);
    fileDescriptor = 0;
    running        = false;
}

#endif // SUPPORT_OSS_DSP


/*------------------------------------------------------------------------------
 
  $Source$

  $Log$
  Revision 1.11  2002/05/28 12:35:41  darkeye
  code cleanup: compiles under gcc-c++ 3.1, using -pedantic option

  Revision 1.10  2001/09/26 16:55:30  darkeye
  BSD port

  Revision 1.9  2001/09/11 15:05:21  darkeye
  added Solaris support

  Revision 1.8  2001/09/02 14:08:40  darkeye
  setting the sound card recording sample rate is now more relaxed
  there is no error reported if the sample rate is not exactly the same

  Revision 1.7  2001/08/30 17:25:56  darkeye
  renamed configure.h to config.h

  Revision 1.6  2000/12/01 15:03:28  darkeye
  bug fix in error reporting

  Revision 1.5  2000/11/17 15:50:48  darkeye
  added -Wall flag to compiler and eleminated new warnings

  Revision 1.4  2000/11/13 20:05:07  darkeye
  changed to workaround to start recording so that it reads one sample
  per channel, as opposed to only one sample (which misalignes the channels)

  Revision 1.3  2000/11/12 13:31:40  darkeye
  added kdoc-style documentation comments

  Revision 1.2  2000/11/05 14:08:28  darkeye
  changed builting to an automake / autoconf environment

  Revision 1.1.1.1  2000/11/05 10:05:53  darkeye
  initial version

  
------------------------------------------------------------------------------*/

