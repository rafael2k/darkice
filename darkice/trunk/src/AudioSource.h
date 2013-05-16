/*------------------------------------------------------------------------------

   Copyright (c) 2000-2007 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : AudioSource.h
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
#ifndef AUDIO_SOURCE_H
#define AUDIO_SOURCE_H

#ifndef __cplusplus
#error This is a C++ include file
#endif

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

/* ============================================================ include files */

#include "Source.h"
#include "Reporter.h"


/* ================================================================ constants */


/* =================================================================== macros */

/*------------------------------------------------------------------------------
 *  Determine the kind of audio device based on the system
 *----------------------------------------------------------------------------*/
#if defined( HAVE_ALSA_LIB )
// we have an ALSA sound system available
#define SUPPORT_ALSA_DSP 1
#endif

#if defined( HAVE_PULSEAUDIO_LIB )
// we have an PULSEAUDIO sound system available
#define SUPPORT_PULSEAUDIO_DSP 1
#endif

#if defined( HAVE_SYS_SOUNDCARD_H )
// we have an OSS DSP sound source device available
#define SUPPORT_OSS_DSP 1
#endif

#if defined( HAVE_SYS_AUDIO_H ) || defined( HAVE_SYS_AUDIOIO_H )
// we have a Solaris DSP sound device available (same for OpenBSD)
#define SUPPORT_SOLARIS_DSP 1
#endif

#if defined( HAVE_JACK_LIB )
// we have JACK audio server
#define SUPPORT_JACK_DSP 1
#endif

#if defined ( HAVE_TERMIOS_H )
#define SUPPORT_SERIAL_ULAW 1
#endif

#if !defined( SUPPORT_ALSA_DSP ) \
    && !defined( SUPPORT_PULSEAUDIO_DSP ) \
    && !defined( SUPPORT_OSS_DSP ) \
    && !defined( SUPPORT_JACK_DSP ) \
    && !defined( SUPPORT_SOLARIS_DSP ) \
    && !defined( SUPPORT_SERIAL_ULAW)
// there was no DSP audio system found
#error No DSP audio input device found on system
#endif


/* =============================================================== data types */

/**
 *  Audio data input
 *
 *  @author  $Author: rafael@riseup.net $
 *  @version $Revision: 488 $
 */
class AudioSource : public Source, public virtual Reporter
{
    private:

        /**
         *  Number of channels of the audio source
         *  (e.g. 1 for mono, 2 for stereo, etc.)
         */
        unsigned int    channel;

        /**
         *  Samples per second (e.g. 44100 for 44.1kHz CD quality)
         */
        unsigned int    sampleRate;

        /**
         *  Bits per sample (e.g. 8 bits, 16 bits, etc.)
         */
        unsigned int    bitsPerSample;

        /**
         *  Initialize the object.
         *
         *  @param sampleRate samples per second.
         *  @param bitsPerSample bits per sample.
         *  @param channel number of channels of the audio source.
         *  @exception Exception
         */
        inline void
        init (   unsigned int   sampleRate,
                 unsigned int   bitsPerSample,
                 unsigned int   channel )               throw ( Exception )
        {
            this->sampleRate     = sampleRate;
            this->bitsPerSample  = bitsPerSample;
            this->channel        = channel;
        }

        /**
         *  De-initialize the object.
         *
         *  @exception Exception
         */
        inline void
        strip ( void )                                  throw ( Exception )
        {
        }


    protected:

        /**
         *  Constructor.
         *  Because all values have defaults, this is also the default
         *  constructor.
         *
         *  @param sampleRate samples per second (e.g. 44100 for 44.1kHz).
         *  @param bitsPerSample bits per sample (e.g. 16 bits).
         *  @param channel number of channels of the audio source
         *                 (e.g. 1 for mono, 2 for stereo, etc.).
         *  @exception Exception
         */
        inline
        AudioSource (   unsigned int    sampleRate    = 44100,
                        unsigned int    bitsPerSample = 16,
                        unsigned int    channel       = 2 )
                                                        throw ( Exception )
        {
            init ( sampleRate, bitsPerSample, channel);
        }

        /**
         *  Copy Constructor.
         *
         *  @param as the object to copy.
         *  @exception Exception
         */
        inline
        AudioSource (   const AudioSource &     as )    throw ( Exception )
            : Source( as )
        {
            init ( as.sampleRate, as.bitsPerSample, as.channel);
        }

        /**
         *  Assignment operator.
         *
         *  @param as the object to assign to this one.
         *  @return a reference to this object.
         *  @exception Exception
         */
        inline virtual AudioSource &
        operator= (     const AudioSource &     as )    throw ( Exception )
        {
            if ( this != &as ) {
                strip();
                Source::operator=( as );
                init ( as.sampleRate, as.bitsPerSample, as.channel);
            }

            return *this;
        }


    public:

        /**
         *  Destructor.
         *
         *  @exception Exception
         */
        virtual inline
        ~AudioSource ( void )                           throw ( Exception )
        {
        }

        /**
         *  Get the number of channels for this AudioSource.
         *
         *  @return the number of channels.
         */
        inline unsigned int
        getChannel ( void ) const           throw ()
        {
            return channel;
        }

        /**
         *  Tell if the data from this source comes in big or little endian.
         *
         *  @return true if the data is big endian, false if little endian
         */
        virtual bool
        isBigEndian ( void ) const           throw ()
        {
#ifdef WORDS_BIGENDIAN
            return true;
#else
            return false;
#endif
        }

        /**
         *  Get the sample rate per seconds for this AudioSource.
         *
         *  @return the sample rate per seconds.
         */
        inline unsigned int
        getSampleRate ( void ) const        throw ()
        {
            return sampleRate;
        }


        /**
         *  Get the number of bits per sample for this AudioSource.
         *
         *  @return the number of bits per sample.
         */
        inline unsigned int
        getBitsPerSample ( void ) const     throw ()
        {
            return bitsPerSample;
        }

        /**
         *  Factory method for creating an AudioSource object of the
         *  appropriate type, based on the compiled DSP support and
         *  the supplied DSP name parameter.
         *
         *  @param deviceName the audio device (/dev/dspX, hwplug:0,0, etc)
         *  @param jackClientName the source name for jack server
         *  @param paSourceName the pulse audio source
         *  @param sampleRate samples per second (e.g. 44100 for 44.1kHz).
         *  @param bitsPerSample bits per sample (e.g. 16 bits).
         *  @param channel number of channels of the audio source
         *                 (e.g. 1 for mono, 2 for stereo, etc.).
         *  @exception Exception
         */
        static AudioSource *
        createDspSource( const char    * deviceName,
                         const char    * jackClientName,
                         const char    * paSourceName,
                         int             sampleRate    = 44100,
                         int             bitsPerSample = 16,
                         int             channel       = 2) throw ( Exception );

};


/* ================================================= external data structures */

/*------------------------------------------------------------------------------
 *  Determine the kind of audio device based on the system
 *----------------------------------------------------------------------------*/
#if defined( SUPPORT_ALSA_DSP )
#include "AlsaDspSource.h"
#endif

#if defined( SUPPORT_PULSEAUDIO_DSP )
#include "PulseAudioDspSource.h"
#endif

#if defined( SUPPORT_OSS_DSP )
#include "OssDspSource.h"
#endif

#if defined( SUPPORT_SOLARIS_DSP )
#include "SolarisDspSource.h"
#endif

#if defined( SUPPORT_JACK_DSP )
#include "JackDspSource.h"
#endif

#if defined ( SUPPORT_SERIAL_ULAW )
#include "SerialUlaw.h"
#endif


/* ====================================================== function prototypes */



#endif  /* AUDIO_SOURCE_H */

