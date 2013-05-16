/*------------------------------------------------------------------------------

   Copyright (c) 2000-2007 Tyrell Corporation. All rights reserved.

   Copyright (c) 2005 Nicholas Humfrey. All rights reserved.

   Tyrell DarkIce

   File     : JackDspSource.h
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
#ifndef JACK_DSP_SOURCE_H
#define JACK_DSP_SOURCE_H

#ifndef __cplusplus
#error This is a C++ include file
#endif


/* ============================================================ include files */


#include "Reporter.h"
#include "AudioSource.h"

#if defined( HAVE_JACK_LIB )
#include <jack/jack.h>
#include <jack/ringbuffer.h>
#else
#error configure for JACK
#endif


/* ================================================================ constants */


/* =================================================================== macros */


/* =============================================================== data types */

/**
 *  An audio input based on JACK
 *
 *  @author  $Author: rafael@riseup.net $
 *  @version $Revision: 474 $
 */
class JackDspSource : public AudioSource, public virtual Reporter
{
    private:
        /**
         *  The jack client name.
         */
        const char                   * jack_client_name;

        /**
         *  The jack port
         */
        jack_port_t                  * ports[2];

        /**
         *  The jack ring buffer.
         */
        jack_ringbuffer_t            * rb[2];

        /**
         *  The jack client.
         */
        jack_client_t                * client;

        /**
         *  The jack audio sample buffer.
         */
        jack_default_audio_sample_t * tmp_buffer;
        
         /**
         *  Automatically connect the jack ports ? (default is to not)
         */
        bool                auto_connect;

    protected:

        /**
         *  Default constructor. Always throws an Exception.
         *
         *  @exception Exception
         */
        inline
        JackDspSource ( void )                       throw ( Exception )
        {
            throw Exception( __FILE__, __LINE__);
        }

        /**
         *  Initialize the object
         *
         *  @exception Exception
         */
        void
        init ( const char* name )                   throw ( Exception );

        /**
         *  De-initialize the object
         *
         *  @exception Exception
         */
        void
        strip ( void )                              throw ( Exception );


        /**
         *  Attempt to connect up the JACK ports automatically
         */
        void
        do_auto_connect( )                          throw ( Exception );

        /**
         *  Callback called by JACK when audio is available
         */
        static int
        process_callback( jack_nframes_t nframes, void *arg );


        /**
         *  Callback called by JACK when jackd is shutting down
         */
        static void
        shutdown_callback( void *arg );

    public:

        /**
         *  Constructor.
         *
         *  @param name the name of the jack device
         *  @param sampleRate samples per second (e.g. 44100 for 44.1kHz).
         *  @param bitsPerSample bits per sample (e.g. 16 bits).
         *  @param channels number of channels of the audio source
         *                 (e.g. 1 for mono, 2 for stereo, etc.).
         *  @exception Exception
         */
        inline
        JackDspSource ( const char    * name,
                        const char    * jackClientName,
                        int             sampleRate    = 44100,
                        int             bitsPerSample = 16,
                        int             channels      = 2 )
                                                        throw ( Exception )

                    : AudioSource( sampleRate, bitsPerSample, channels )
        {
            jack_client_name = jackClientName;
            init( name );
        }

        /**
         *  Copy Constructor.
         *
         *  @param jds the object to copy.
         *  @exception Exception
         */
        inline
        JackDspSource (  const JackDspSource &    jds )   throw ( Exception )
                    : AudioSource( jds )
        {
            throw Exception( __FILE__, __LINE__, "JackDspSource doesn't copy");
        }

        /**
         *  Destructor.
         *
         *  @exception Exception
         */
        inline virtual
        ~JackDspSource ( void )                          throw ( Exception )
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
        inline virtual JackDspSource &
        operator= (     const JackDspSource &     ds )   throw ( Exception )
        {
            throw Exception( __FILE__, __LINE__, "JackDspSource doesn't assign");
        }

        /**
         *  Open the JackDspSource.
         *  This does not put the Jack DSP device into recording mode.
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
         *  Check if the JackDspSource is registered
         *
         *  @return true if Jack client is setup
         */
        inline virtual bool
        isOpen ( void ) const                           throw ()
        {
            return client != NULL;
        }

        /**
         *  Check if the JackDspSource can be read from.
         *  Blocks until the specified time for data to be available.
         *  Puts the Jack DSP device into recording mode.
         *
         *  @param sec the maximum seconds to block.
         *  @param usec micro seconds to block after the full seconds.
         *  @return true if the JackDspSource is ready to be read from,
         *          false otherwise.
         *  @exception Exception
         */
        virtual bool
        canRead (               unsigned int    sec,
                                unsigned int    usec )  throw ( Exception );

        /**
         *  Read from the JackDspSource.
         *  Puts the Jack DSP device into recording mode.
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
         *  Close the JackDspSource.
         *
         *  @exception Exception
         */
        virtual void
        close ( void )                                  throw ( Exception );

};


/* ================================================= external data structures */


/* ====================================================== function prototypes */



#endif  /* JACK_DSP_SOURCE_H */

