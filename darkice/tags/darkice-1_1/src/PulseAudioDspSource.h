/*------------------------------------------------------------------------------

   Copyright (c) 2000-2007 Tyrell Corporation. All rights reserved.

   Copyright (c) 2004 
   LS Informationstechnik (LIKE)
   University of Erlangen Nuremberg
   All rights reserved.

   Tyrell DarkIce

   File     : PulseAudioDspSource.h
   Version  : $Revision: 394 $
   Author   : $Author: darkeye $
   Location : $HeadURL$
   
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
#ifndef PULSEAUDIO_SOURCE_H
#define PULSEAUDIO_SOURCE_H

#ifndef __cplusplus
#error This is a C++ include file
#endif

/* ============================================================ include files */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Reporter.h"
#include "AudioSource.h"

#ifdef HAVE_PULSEAUDIO_LIB

#include <pulse/simple.h>
#include <pulse/error.h>
#include <pulse/gccmacro.h>
#else
#error configure for PULSEAUDIO 
#endif

/* ================================================================ constants */

/* =================================================================== macros */

/* =============================================================== data types */

/**
 *  An audio input based on the PULSEAUDIO sound system 
 *
 *  @author  $Author: darkeye $
 *  @version $Revision: 394 $
 */
class PulseAudioDspSource : public AudioSource, public virtual Reporter
{
    private:

        /**
         *  Name of the capture PCM stream.
         */
        char *sourceName;

        /**
         *  Handle for PulseAudio 
         */
        pa_simple *s ;

        /**
         * format definitions for pulseaudio
          */
        pa_sample_spec ss;

        int error;

    protected:

        /**
         *  Default constructor. Always throws an Exception.
         *
         *  @exception Exception
         */
        inline
        PulseAudioDspSource ( void )                       throw ( Exception )
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
        PulseAudioDspSource (  const char    * paSourceName,
                         int             sampleRate    = 44100,
                         int             bitsPerSample = 16,
                         int             channel       = 2 )
                                                        throw ( Exception )
                    : AudioSource( sampleRate, bitsPerSample, channel)
        {
            init( paSourceName);
        }

        /**
         *  Copy Constructor.
         *
         *  @param ds the object to copy.
         *  @exception Exception
         */
        inline
        PulseAudioDspSource (  const PulseAudioDspSource &    ds )    throw ( Exception )
                    : AudioSource( ds )
        {
            init( ds.sourceName);
        }

        /**
         *  Destructor.
         *
         *  @exception Exception
         */
        inline virtual
        ~PulseAudioDspSource ( void )                          throw ( Exception )
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
        inline virtual PulseAudioDspSource &
        operator= (     const PulseAudioDspSource &     ds )   throw ( Exception )
        {
            if ( this != &ds ) {
                strip();
                AudioSource::operator=( ds);
                init( ds.sourceName);
            }
            return *this;
        }

        /**
         *  Open the PulseAudioDspSource.
         *
         *  @return true if opening was successful, false otherwise
         *  @exception Exception
         */
        virtual bool
        open ( void )                                   throw ( Exception );

        /**
         *  Check if the PulseAudioDspSource is open.
         *
         *  @return true if the PulseAudioDspSource is open, false otherwise.
         */
        inline virtual bool
        isOpen ( void ) const                           throw ()
        {
            return s==NULL;
        }

        /**
         *  Check if the PulseAudioDspSource can be read from.
         *
         *  @param sec the maximum seconds to block.
         *  @param usec micro seconds to block after the full seconds.
         *  @return true if the PulseAudioDspSource is ready to be read from,
         *          false otherwise.
         *  @exception Exception
         */
        virtual bool
        canRead (               unsigned int    sec,
                                unsigned int    usec )  throw ( Exception );

        /**
         *  Read from the PulseAudioDspSource.
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
         *  Close the PulseAudioDspSource.
         *
         *  @exception Exception
         */
        virtual void
        close ( void )                                  throw ( Exception );

};

/* ================================================= external data structures */

/* ====================================================== function prototypes */

#endif  /* PULSEAUDIO_SOURCE_H */
/*------------------------------------------------------------------------------

   Copyright (c) 2000-2007 Tyrell Corporation. All rights reserved.

   Copyright (c) 2004 
   LS Informationstechnik (LIKE)
   University of Erlangen Nuremberg
   All rights reserved.

   Tyrell DarkIce

   File     : PulseAudioDspSource.h
   Version  : $Revision: 394 $
   Author   : $Author: darkeye $
   Location : $HeadURL$
   
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
#ifndef PULSEAUDIO_SOURCE_H
#define PULSEAUDIO_SOURCE_H

#ifndef __cplusplus
#error This is a C++ include file
#endif

/* ============================================================ include files */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Reporter.h"
#include "AudioSource.h"

#ifdef HAVE_PULSEAUDIO_LIB

#include <pulse/simple.h>
#include <pulse/error.h>
#include <pulse/gccmacro.h>
#else
#error configure for PULSEAUDIO 
#endif

/* ================================================================ constants */

/* =================================================================== macros */

/* =============================================================== data types */

/**
 *  An audio input based on the PULSEAUDIO sound system 
 *
 *  @author  $Author: darkeye $
 *  @version $Revision: 394 $
 */
class PulseAudioDspSource : public AudioSource, public virtual Reporter
{
    private:

        /**
         *  Name of the capture PCM stream.
         */
        char *sourceName;

        /**
         *  Handle for PulseAudio 
         */
        pa_simple *s ;

        /**
         * format definitions for pulseaudio
          */
        pa_sample_spec ss;

        int error;

    protected:

        /**
         *  Default constructor. Always throws an Exception.
         *
         *  @exception Exception
         */
        inline
        PulseAudioDspSource ( void )                       throw ( Exception )
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
        PulseAudioDspSource (  const char    * paSourceName,
                         int             sampleRate    = 44100,
                         int             bitsPerSample = 16,
                         int             channel       = 2 )
                                                        throw ( Exception )
                    : AudioSource( sampleRate, bitsPerSample, channel)
        {
            init( paSourceName);
        }

        /**
         *  Copy Constructor.
         *
         *  @param ds the object to copy.
         *  @exception Exception
         */
        inline
        PulseAudioDspSource (  const PulseAudioDspSource &    ds )    throw ( Exception )
                    : AudioSource( ds )
        {
            init( ds.sourceName);
        }

        /**
         *  Destructor.
         *
         *  @exception Exception
         */
        inline virtual
        ~PulseAudioDspSource ( void )                          throw ( Exception )
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
        inline virtual PulseAudioDspSource &
        operator= (     const PulseAudioDspSource &     ds )   throw ( Exception )
        {
            if ( this != &ds ) {
                strip();
                AudioSource::operator=( ds);
                init( ds.sourceName);
            }
            return *this;
        }

        /**
         *  Open the PulseAudioDspSource.
         *
         *  @return true if opening was successful, false otherwise
         *  @exception Exception
         */
        virtual bool
        open ( void )                                   throw ( Exception );

        /**
         *  Check if the PulseAudioDspSource is open.
         *
         *  @return true if the PulseAudioDspSource is open, false otherwise.
         */
        inline virtual bool
        isOpen ( void ) const                           throw ()
        {
            return s==NULL;
        }

        /**
         *  Check if the PulseAudioDspSource can be read from.
         *
         *  @param sec the maximum seconds to block.
         *  @param usec micro seconds to block after the full seconds.
         *  @return true if the PulseAudioDspSource is ready to be read from,
         *          false otherwise.
         *  @exception Exception
         */
        virtual bool
        canRead (               unsigned int    sec,
                                unsigned int    usec )  throw ( Exception );

        /**
         *  Read from the PulseAudioDspSource.
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
         *  Close the PulseAudioDspSource.
         *
         *  @exception Exception
         */
        virtual void
        close ( void )                                  throw ( Exception );

};

/* ================================================= external data structures */

/* ====================================================== function prototypes */

#endif  /* PULSEAUDIO_SOURCE_H */
