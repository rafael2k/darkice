/*------------------------------------------------------------------------------

   Copyright (c) 2000-2007 Tyrell Corporation. All rights reserved.

   Copyright (c) 2004
   LS Informationstechnik (LIKE)
   University of Erlangen Nuremberg
   All rights reserved.

   Tyrell DarkIce

   File     : AlsaDspSource.cpp
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

#include "AudioSource.h"

// compile only if configured for ALSA
#ifdef SUPPORT_ALSA_DSP

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Util.h"
#include "Exception.h"
#include "AlsaDspSource.h"


/* ===================================================  local data structures */


/* ================================================  local constants & macros */

/*------------------------------------------------------------------------------
 *  File identity
 *----------------------------------------------------------------------------*/
static const char fileid[] = "$Id: AlsaDspSource.cpp 474 2010-05-10 01:18:15Z rafael@riseup.net $";


/* ===============================================  local function prototypes */


/* =============================================================  module code */

/*------------------------------------------------------------------------------
 *  Tell if source id big endian
 *----------------------------------------------------------------------------*/
bool
AlsaDspSource :: isBigEndian ( void ) const                  throw ()
{
    return SND_PCM_FORMAT_S16 == SND_PCM_FORMAT_S16_BE;
}


/*------------------------------------------------------------------------------
 *  Initialize the object
 *----------------------------------------------------------------------------*/
void
AlsaDspSource :: init (  const char      * name )    throw ( Exception )
{
    pcmName       = Util::strDup( name);
    captureHandle = 0;
    bufferTime    = 1000000; // Do 1s buffering
    running       = false;
}


/*------------------------------------------------------------------------------
 *  De-initialize the object
 *----------------------------------------------------------------------------*/
void
AlsaDspSource :: strip ( void )                      throw ( Exception )
{
    if ( isOpen() ) {
        close();
    }

    delete[] pcmName;
}


/*------------------------------------------------------------------------------
 *  Open the audio source
 *----------------------------------------------------------------------------*/
bool
AlsaDspSource :: open ( void )                       throw ( Exception )
{
    unsigned int        u;
    snd_pcm_format_t    format;
    snd_pcm_hw_params_t *hwParams;

    if ( isOpen() ) {
        return false;
    }

    switch ( getBitsPerSample() ) {
        case 8:
            format = SND_PCM_FORMAT_S8;
            break;

        case 16:
            format = SND_PCM_FORMAT_S16;
            break;
            
        default:
            return false;
    }

    if (snd_pcm_open(&captureHandle, pcmName, SND_PCM_STREAM_CAPTURE, 0) < 0) {
        captureHandle = 0;
        return false;
    }

    if (snd_pcm_hw_params_malloc(&hwParams) < 0) {
        close();
        throw Exception( __FILE__, __LINE__, "can't alloc hardware "\
                        "parameter structure");
    }

    if (snd_pcm_hw_params_any(captureHandle, hwParams) < 0) {
        snd_pcm_hw_params_free(hwParams);
        close();
        throw Exception( __FILE__, __LINE__, "can't initialize hardware "\
                        "parameter structure");
    }

    if (snd_pcm_hw_params_set_access(captureHandle, hwParams,
                                     SND_PCM_ACCESS_RW_INTERLEAVED) < 0) {
        snd_pcm_hw_params_free(hwParams);
        close();
        throw Exception( __FILE__, __LINE__, "can't set access type");
    }

    if (snd_pcm_hw_params_set_format(captureHandle, hwParams, format) < 0) {
        snd_pcm_hw_params_free(hwParams);
        close();
        throw Exception( __FILE__, __LINE__, "can't set sample format");
    }

    u = getSampleRate();
    if (snd_pcm_hw_params_set_rate_near(captureHandle, hwParams, &u, 0) < 0) {
        snd_pcm_hw_params_free(hwParams);
        close();
        throw Exception( __FILE__, __LINE__, "can't set sample rate", u);
    }

    u = getChannel();
    if (snd_pcm_hw_params_set_channels(captureHandle, hwParams, u) < 0) {
        snd_pcm_hw_params_free(hwParams);
        close();
        throw Exception( __FILE__, __LINE__, "can't set channels", u);
    }

    u = 4;
    if (snd_pcm_hw_params_set_periods_near(captureHandle, hwParams, &u, 0)
                                                                          < 0) {
        snd_pcm_hw_params_free(hwParams);
        close();
        throw Exception( __FILE__, __LINE__, "can't set interrupt frequency");
    }

    u = getBufferTime();
    if (snd_pcm_hw_params_set_buffer_time_near(captureHandle, hwParams, &u, 0)
                                                                          < 0) {
        snd_pcm_hw_params_free(hwParams);
        close();
        throw Exception( __FILE__, __LINE__, "can't set buffer size");
    }

    if (snd_pcm_hw_params(captureHandle, hwParams) < 0) {
        snd_pcm_hw_params_free(hwParams);
        close();
        throw Exception( __FILE__, __LINE__, "can't set hardware parameters");
    }

    snd_pcm_hw_params_free(hwParams);

    if (snd_pcm_prepare(captureHandle) < 0) {
        close();
        throw Exception( __FILE__, __LINE__, "can't prepare audio interface "\
                        "for use");
    }

    bytesPerFrame = getChannel() * getBitsPerSample() / 8;

    return true;
}


/*------------------------------------------------------------------------------
 *  Check wether read() would return anything
 *----------------------------------------------------------------------------*/
bool
AlsaDspSource :: canRead ( unsigned int    sec,
                           unsigned int    usec )    throw ( Exception )
{
    if ( !isOpen() ) {
        return false;
    }

    if ( !running ) {
        snd_pcm_start(captureHandle); 
        running = true;
    }

    /*
     * FIXME How to check for available frames? It
     * seems like snd_pcm_wait stops working when
     * it comes to ALSA plugins... :-(
     *
     * int milliDelay = sec * 1000 + usec/1000;
     * return snd_pcm_wait(captureHandle, milliDelay)!=0;
     */
    return true; // bad!!
}


/*------------------------------------------------------------------------------
 *  Read from the audio source
 *----------------------------------------------------------------------------*/
unsigned int
AlsaDspSource :: read (    void          * buf,
                           unsigned int    len )     throw ( Exception )
{
    snd_pcm_sframes_t ret;

    if ( !isOpen() ) {
        return 0;
    }

    do {
        ret = snd_pcm_readi(captureHandle, buf, len/bytesPerFrame);

        // Check for buffer overrun
        if (ret == -EPIPE) {
            reportEvent(1, "Buffer overrun!");
            snd_pcm_prepare(captureHandle);
            ret = -EAGAIN;
        }
    } while (ret == -EAGAIN);

    if ( ret < 0 ) {
        throw Exception(__FILE__, __LINE__, snd_strerror(ret));
    }

    running = true;
    return ret * bytesPerFrame;
}


/*------------------------------------------------------------------------------
 *  Close the audio source
 *----------------------------------------------------------------------------*/
void
AlsaDspSource :: close ( void )                  throw ( Exception )
{
    if ( !isOpen() ) {
        return;
    }

    snd_pcm_close(captureHandle);

    captureHandle  = 0;
    running        = false;
}

#endif // HAVE_ALSA_LIB

