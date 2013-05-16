/*------------------------------------------------------------------------------

   Copyright (c) 2000-2007 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : AudioSource.cpp
   Version  : $Revision: 488 $
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "AudioSource.h"
#include "Util.h"
#include "Exception.h"


/* ===================================================  local data structures */


/* ================================================  local constants & macros */

/*------------------------------------------------------------------------------
 *  File identity
 *----------------------------------------------------------------------------*/
static const char fileid[] = "$Id: AudioSource.cpp 488 2010-09-17 20:37:46Z rafael@riseup.net $";


/* ===============================================  local function prototypes */


/* =============================================================  module code */

/*------------------------------------------------------------------------------
 *  Return an audio source based on the compiled DSP supports and the
 *  supplied device name parameter.
 *----------------------------------------------------------------------------*/
AudioSource *
AudioSource :: createDspSource( const char    * deviceName,
                                const char    * jackClientName,
                                const char    * paSourceName,
                                int             sampleRate,
                                int             bitsPerSample,
                                int             channel)
                                                            throw ( Exception )
{
    
    if ( Util::strEq( deviceName, "/dev/tty", 8) ) {
#if defined( SUPPORT_SERIAL_ULAW )
        Reporter::reportEvent( 1, "Using Serial Ulaw input device:",
                                  deviceName);
        return new SerialUlaw( deviceName,
                                 sampleRate,
                                 bitsPerSample,
                                 channel);
#else
        throw Exception( __FILE__, __LINE__,
                             "trying to open Serial ULaw device "
                             "without support compiled", deviceName);
#endif
	} else if ( Util::strEq( deviceName, "/dev", 4) ) {
#if defined( SUPPORT_OSS_DSP )
        Reporter::reportEvent( 1, "Using OSS DSP input device:", deviceName);
        return new OssDspSource( deviceName,
                                 sampleRate,
                                 bitsPerSample,
                                 channel);
#elif defined( SUPPORT_SOLARIS_DSP )
        Reporter::reportEvent( 1, "Using Solaris DSP input device:",deviceName);
        return new SolarisDspSource( deviceName,
                                     sampleRate,
                                     bitsPerSample,
                                     channel);
#else
        throw Exception( __FILE__, __LINE__,
                             "trying to open OSS or Solaris DSP device "
                             "without support compiled", deviceName);
#endif
	} else if ( Util::strEq( deviceName, "jack", 4) ) {
#if defined( SUPPORT_JACK_DSP )
        Reporter::reportEvent( 1, "Using JACK audio server as input device.");
        return new JackDspSource( deviceName,
                                  jackClientName,
                                  sampleRate,
                                  bitsPerSample,
                                  channel);
#else
        throw Exception( __FILE__, __LINE__,
                             "trying to open JACK device without "
                             "support compiled", deviceName);
#endif
	} else if ( Util::strEq( deviceName, "pulseaudio", 10) ) {
#if defined( SUPPORT_PULSEAUDIO_DSP )
        Reporter::reportEvent( 1, "Using PulseAudio audio server as input device.");
        return new PulseAudioDspSource( paSourceName,
                                        sampleRate,
                                        bitsPerSample,
                                        channel);
#else
        throw Exception( __FILE__, __LINE__,
                             "trying to open PulseAudio device without "
                             "support compiled", deviceName);
#endif
    } else {
#if defined( SUPPORT_ALSA_DSP )
        Reporter::reportEvent( 1, "Using ALSA DSP input device:", deviceName);
        return new AlsaDspSource( deviceName,
                                  sampleRate,
                                  bitsPerSample,
                                  channel);
#else
        throw Exception( __FILE__, __LINE__,
                             "trying to open ALSA DSP device without "
                             "support compiled", deviceName);
#endif
    }
}

