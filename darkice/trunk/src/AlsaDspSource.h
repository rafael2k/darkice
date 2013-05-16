/*------------------------------------------------------------------------------

   Copyright (c) 2000-2007 Tyrell Corporation. All rights reserved.

   Copyright (c) 2004 
   LS Informationstechnik (LIKE)
   University of Erlangen Nuremberg
   All rights reserved.

   Tyrell DarkIce

   File     : AlsaDspSource.h
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
#ifndef ALSA_SOURCE_H
#define ALSA_SOURCE_H

#ifndef __cplusplus
#error This is a C++ include file
#endif


/* ============================================================ include files */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Reporter.h"
#include "AudioSource.h"

#ifdef HAVE_ALSA_LIB
#include <alsa/asoundlib.h>
#else
#error configure for ALSA 
#endif


/* ================================================================ constants */


/* =================================================================== macros */


/* =============================================================== data types */

/**
 *  An audio input based on the ALSA sound system 
 *
 *  @author  $Author: rafael@riseup.net $
 *  @version $Revision: 474 $
 */
class AlsaDspSource : public AudioSource, public virtual Reporter
{
    private:

        /**
         *  Name of the capture PCM stream.
         */
        char *pcmName;

        /**
         *  Handle to access PCM stream data.
         */
        snd_pcm_t *captureHandle;

        /**
         *  Stores number of bytes per frame. One frame
         *  contains all samples per time instance.
         */
        int bytesPerFrame;

        /**
         *  Is the stream running?
         */
        bool running;

        /**
         *  Number of useconds to do buffering in the audio device.
         */
        unsigned int bufferTime;


    protected:

        /**
         *  Default constructor. Always throws an Exception.
         *
         *  @exception Exception
         */
        inline
        AlsaDspSource ( void )                       throw ( Exception )
        {
            throw Exception( __FILE__, __LINE__);
        }

        /**
         *  Initialize the object
         *
         *  @param name the PCM to open.
         *  @exception Exception
         */
        void
        init (  const char    * name )              throw ( Exception );

        /**
         *  De-iitialize the object
         *
         *  @exception Exception
         */
        void
        strip ( void )                              throw ( Exception );


    public:

        /**
         *  Constructor.
         *
         *  @param name the PCM (e.g. "hwplug:0,0").
         *  @param sampleRate samples per second (e.g. 44100 for 44.1kHz).
         *  @param bitsPerSample bits per sample (e.g. 16 bits).
         *  @param channel number of channels of the audio source
         *                 (e.g. 1 for mono, 2 for stereo, etc.).
         *  @exception Exception
         */
        inline
        AlsaDspSource (  const char    * name,
                         int             sampleRate    = 44100,
                         int             bitsPerSample = 16,
                         int             channel       = 2 )
                                                        throw ( Exception )
                    : AudioSource( sampleRate, bitsPerSample, channel)
        {
            init( name);
        }

        /**
         *  Copy Constructor.
         *
         *  @param ds the object to copy.
         *  @exception Exception
         */
        inline
        AlsaDspSource (  const AlsaDspSource &    ds )    throw ( Exception )
                    : AudioSource( ds )
        {
            init( ds.pcmName);
        }

        /**
         *  Destructor.
         *
         *  @exception Exception
         */
        inline virtual
        ~AlsaDspSource ( void )                          throw ( Exception )
        {
            strip();
        }

        /**
         *  Assignment operator.
         *
         *  @param ds the object to assign to this one.
         *  @return a reference to this object.
         *  @exception Exception
         */
        inline virtual AlsaDspSource &
        operator= (     const AlsaDspSource &     ds )   throw ( Exception )
        {
            if ( this != &ds ) {
                strip();
                AudioSource::operator=( ds);
                init( ds.pcmName);
            }
            return *this;
        }

        /**
         *  Tell if the data from this source comes in big or little endian.
         *
         *  @return true if the source is big endian, false otherwise
         */
        virtual bool
        isBigEndian ( void ) const           throw ();

        /**
         *  Open the AlsaDspSource.
         *  This does not put Alsa device into recording mode.
         *  To start getting samples, call either canRead() or read().
         *
         *  @return true if opening was successful, false otherwise
         *  @exception Exception
         *  
         *  @see #canRead
         *  @see #read
         */
        virtual bool
        open ( void )                                   throw ( Exception );

        /**
         *  Check if the AlsaDspSource is open.
         *
         *  @return true if the AlsaDspSource is open, false otherwise.
         */
        inline virtual bool
        isOpen ( void ) const                           throw ()
        {
            return captureHandle != 0;
        }

        /**
         *  Check if the AlsaDspSource can be read from.
         *  Blocks until the specified time for data to be available.
         *  Puts the PCM into recording mode.
         *
         *  @param sec the maximum seconds to block.
         *  @param usec micro seconds to block after the full seconds.
         *  @return true if the AlsaDspSource is ready to be read from,
         *          false otherwise.
         *  @exception Exception
         */
        virtual bool
        canRead (               unsigned int    sec,
                                unsigned int    usec )  throw ( Exception );

        /**
         *  Read from the AlsaDspSource.
         *  Puts the PCM into recording mode.
         *
         *  @param buf the buffer to read into.
         *  @param len the number of bytes to read into buf
         *  @return the number of bytes read (may be less than len).
         *  @exception Exception
         */
        virtual unsigned int
        read (                  void          * buf,
                                unsigned int    len )   throw ( Exception );

        /**
         *  Close the AlsaDspSource.
         *
         *  @exception Exception
         */
        virtual void
        close ( void )                                  throw ( Exception );

        /**
         *  Returns the buffer size in useconds.
         *
         *  @return the number of useconds audio will be buffered in ALSA
         */
        inline virtual unsigned int
        getBufferTime( void ) const
        { 
            return bufferTime;
        }

        /**
         *  Sets the number of useconds to buffer audio in ALSA 
         *
         *  @param time buffer time
         */
        inline virtual void
        setBufferTime( unsigned int time ) {
            bufferTime = time;
        }
};


/* ================================================= external data structures */


/* ====================================================== function prototypes */



#endif  /* ALSA_SOURCE_H */

