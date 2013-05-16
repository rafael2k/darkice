/*------------------------------------------------------------------------------

   Copyright (c) 2000-2007 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : SolarisDspSource.cpp
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

/* ============================================================ include files */

#include "SolarisDspSource.h"

#ifdef SUPPORT_SOLARIS_DSP
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

#ifdef HAVE_SIGNAL_H
#include <signal.h>
#else
#error need signal.h
#endif

#if defined( HAVE_SYS_AUDIO_H )
#include <sys/audio.h>
#elif defined( HAVE_SYS_AUDIOIO_H )
#include <sys/audioio.h>
#else
#error need sys/audio.h or sys/audioio.h
#endif


#include "Util.h"
#include "Exception.h"
#include "SolarisDspSource.h"


/* ===================================================  local data structures */


/* ================================================  local constants & macros */

/*------------------------------------------------------------------------------
 *  File identity
 *----------------------------------------------------------------------------*/
static const char fileid[] = "$Id: SolarisDspSource.cpp 474 2010-05-10 01:18:15Z rafael@riseup.net $";


/* ===============================================  local function prototypes */


/* =============================================================  module code */

/*------------------------------------------------------------------------------
 *  Initialize the object
 *----------------------------------------------------------------------------*/
void
SolarisDspSource :: init (  const char      * name )    throw ( Exception )
{
    fileName       = Util::strDup( name);
    fileDescriptor = 0;
}


/*------------------------------------------------------------------------------
 *  De-initialize the object
 *----------------------------------------------------------------------------*/
void
SolarisDspSource :: strip ( void )                      throw ( Exception )
{
    if ( isOpen() ) {
        close();
    }
    
    delete[] fileName;
}

#include <errno.h>

/*------------------------------------------------------------------------------
 *  Open the audio source
 *----------------------------------------------------------------------------*/
bool
SolarisDspSource :: open ( void )                       throw ( Exception )
{
    audio_info_t    audioInfo;

    if ( isOpen() ) {
        return false;
    }

    if ( (fileDescriptor = ::open( fileName, O_RDONLY)) == -1 ) {
        fileDescriptor = 0;
        return false;
    }

    AUDIO_INITINFO( &audioInfo);
    audioInfo.record.sample_rate = getSampleRate();
    audioInfo.record.channels    = getChannel();
    audioInfo.record.precision   = getBitsPerSample();
    audioInfo.record.encoding    = AUDIO_ENCODING_LINEAR;
    // for stupid OpenBSD we need to add the following, as it masks
    // read/write calls when using -pthread
    audioInfo.record.pause       = 0;

    if ( ioctl( fileDescriptor, AUDIO_SETINFO, &audioInfo) == -1 ) {

        close();
        throw Exception( __FILE__, __LINE__, "ioctl error");
    }

    if ( audioInfo.record.channels != getChannel() ) {
        close();
        throw Exception( __FILE__, __LINE__,
                         "can't set channels", audioInfo.record.channels);
    }

    if ( audioInfo.record.precision != getBitsPerSample() ) {
        close();
        throw Exception( __FILE__, __LINE__,
                         "can't set bits per sample",
                         audioInfo.record.precision);
    }

    if ( audioInfo.record.sample_rate != getSampleRate() ) {
        reportEvent( 2, "sound card recording sample rate set to ",
                        audioInfo.record.sample_rate,
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
SolarisDspSource :: canRead ( unsigned int    sec,
                              unsigned int    usec )    throw ( Exception )
{
    fd_set              fdset;
    struct timespec     timespec;
    sigset_t            sigset;
    int                 ret;

    if ( !isOpen() ) {
        return false;
    }

    FD_ZERO( &fdset);
    FD_SET( fileDescriptor, &fdset);

    timespec.tv_sec  = sec;
    timespec.tv_nsec = usec * 1000L;

    // mask out SIGUSR1, as we're expecting that signal for other reasons
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGUSR1);

    ret = pselect( fileDescriptor + 1, &fdset, NULL, NULL, &timespec, &sigset);
    
    if ( ret == -1 ) {
        throw Exception( __FILE__, __LINE__, "select error");
    }

    return ret > 0;
}


/*------------------------------------------------------------------------------
 *  Read from the audio source
 *----------------------------------------------------------------------------*/
unsigned int
SolarisDspSource :: read (    void          * buf,
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

    return ret;
}


/*------------------------------------------------------------------------------
 *  Close the audio source
 *----------------------------------------------------------------------------*/
void
SolarisDspSource :: close ( void )                  throw ( Exception )
{
    if ( !isOpen() ) {
        return;
    }

    ::close( fileDescriptor);
    fileDescriptor = 0;
}

#endif // SUPPORT_SOLARIS_DSP

