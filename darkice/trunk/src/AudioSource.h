/*------------------------------------------------------------------------------

   Copyright (c) 2000 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : AudioSource.h
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
#ifndef AUDIO_SOURCE_H
#define AUDIO_SOURCE_H

#ifndef __cplusplus
#error This is a C++ include file
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
#define SUPPORT_ALSA_DSP
#endif

#if defined( HAVE_SYS_SOUNDCARD_H )
// we have an OSS DSP sound source device available
#define SUPPORT_OSS_DSP
#endif

#if defined( HAVE_SYS_AUDIO_H ) || defined( HAVE_SYS_AUDIOIO_H )
// we have a Solaris DSP sound device available (same for OpenBSD)
#define SUPPORT_SOLARIS_DSP
#endif

#if !defined( SUPPORT_ALSA_DSP ) \
    && !defined( SUPPORT_OSS_DSP ) \
    && !defined( SUPPORT_SOLARIS_DSP )
// there was no DSP audio system found
#error No DSP audio input device found on system
#endif


/* =============================================================== data types */

/**
 *  Audio data input
 *
 *  @author  $Author$
 *  @version $Revision$
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
         *  @param source the object to copy.
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
        isBigEndian ( void ) const           throw ()    = 0;

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
         *  @param name the audio device (/dev/dspX, hwplug:0,0, etc)
         *  @param sampleRate samples per second (e.g. 44100 for 44.1kHz).
         *  @param bitsPerSample bits per sample (e.g. 16 bits).
         *  @param channel number of channels of the audio source
         *                 (e.g. 1 for mono, 2 for stereo, etc.).
         *  @exception Exception
         */
        static AudioSource *
        createDspSource( const char    * deviceName,
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

#if defined( SUPPORT_OSS_DSP )
#include "OssDspSource.h"
#endif

#if defined( SUPPORT_SOLARIS_DSP )
#include "SolarisDspSource.h"
#endif


/* ====================================================== function prototypes */



#endif  /* AUDIO_SOURCE_H */


/*------------------------------------------------------------------------------
 
  $Source$

  $Log$
  Revision 1.7  2004/02/18 21:08:11  darkeye
  ported to OpenBSD (real-time scheduling not yet supported)

  Revision 1.6  2004/02/15 12:06:29  darkeye
  added ALSA support, thanks to Christian Forster

  Revision 1.5  2001/09/18 14:57:19  darkeye
  finalized Solaris port

  Revision 1.4  2001/09/11 15:05:21  darkeye
  added Solaris support

  Revision 1.3  2000/11/12 13:31:40  darkeye
  added kdoc-style documentation comments

  Revision 1.2  2000/11/05 17:37:24  darkeye
  removed clone() functions

  Revision 1.1.1.1  2000/11/05 10:05:47  darkeye
  initial version

  
------------------------------------------------------------------------------*/

