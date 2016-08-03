/*------------------------------------------------------------------------------

   Copyright (c) 2000-2007 Tyrell Corporation. All rights reserved.

   Copyright (c) 2005 Nicholas Humfrey. All rights reserved.

   Tyrell DarkIce

   File     : JackDspSource.cpp
   Version  : $Revision$
   Author   : $Author$
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

#ifdef SUPPORT_JACK_DSP
// only compile this code if there is support for it

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_STDIO_H
#include <stdio.h>
#else
#error need stdio.h
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

#ifdef HAVE_MATH_H
#include <math.h>
#else
#error need math.h
#endif

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#else
#error needs stdlib.h
#endif

#ifdef HAVE_LIMITS_H
#include <limits.h>
#else
#error need limits.h
#endif

#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif

#include <climits>

#include "Util.h"
#include "Exception.h"
#include "JackDspSource.h"


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
JackDspSource :: init ( const char* name )           throw ( Exception )
{
    // Set defaults
    ports[0]     = NULL;        // Left Port
    ports[1]     = NULL;        // Right Port
    rb[0]        = NULL;        // Left Ring Buffer
    rb[1]        = NULL;        // Right Ring Buffer
    client       = NULL;
    auto_connect = false;       // Default is to not auto connect the JACK ports
    tmp_buffer   = NULL;        // Buffer big enough for one 'read' of audio

    // Auto connect the ports ?
    if ( Util::strEq( name, "jack_auto", 9) ) {
        auto_connect = true;
    }
    
    // Check the sample size
    if (getBitsPerSample() != 16) {
        throw Exception( __FILE__, __LINE__,
                        "JackDspSource doesn't support non 16-bit samples");
    }
}



/*------------------------------------------------------------------------------
 *  De-initialize the object
 *----------------------------------------------------------------------------*/
void
JackDspSource :: strip ( void )                      throw ( Exception )
{
    if ( isOpen() ) {
        close();
    }
    
    // Free the temporary buffer
    if (tmp_buffer) {
        free(tmp_buffer);
        tmp_buffer = NULL;
    }

}

/*------------------------------------------------------------------------------
 *  Attempt to connect up the JACK ports automatically
 *   - Just connect left&right to the first two output ports we find
 *----------------------------------------------------------------------------*/
void
JackDspSource :: do_auto_connect ( void )                   throw ( Exception )
{
    const char **all_ports;
    unsigned int ch = 0;
    int          i;

    Reporter::reportEvent( 10, "JackDspSource :: do_auto_connect");
    
    // Get a list of all the jack ports
    all_ports = jack_get_ports (client, NULL, NULL, JackPortIsOutput);
    if (!ports) {
        throw Exception( __FILE__, __LINE__, "jack_get_ports() returned NULL.");
    }
    
    // Step through each port name
    for (i = 0; all_ports[i]; ++i) {

        const char* in  = all_ports[i];
        const char* out = jack_port_name( ports[ch] );
        
        Reporter::reportEvent( 2, "Connecting", in, "to", out);
        
        if (jack_connect(client, in, out)) {
            throw Exception( __FILE__, __LINE__,
                            "Failed to jack_connect() ports", in, out);
        }
    
        // Found enough ports ?
        if (++ch >= getChannel()) break;
    }
    
    free( all_ports );

}


/*------------------------------------------------------------------------------
 *  Open the audio source
 *----------------------------------------------------------------------------*/
bool
JackDspSource :: open ( void )                       throw ( Exception )
{
    char         client_name[255];
    size_t       rb_size;
    unsigned int c;
    
    if ( isOpen() ) {
        return false;
    }

    // Register client with Jack
    if ( jack_client_name != NULL ) {
      snprintf(client_name, 255, "%s", jack_client_name);
    } else {
      snprintf(client_name, 255, "darkice-%d", getpid());
    }

    if ((client = jack_client_open(client_name, (jack_options_t)0, NULL)) == NULL) {
        throw Exception( __FILE__, __LINE__, "JACK server not running?");
    }
    Reporter::reportEvent( 1, "Registering as JACK client", client_name);


    // Check the sample rate is correct
    if (jack_get_sample_rate( client ) != (jack_nframes_t)getSampleRate()) {
        throw Exception( __FILE__, __LINE__,
                        "JACK server sample rate is different than "
                        "sample rate in darkice config file");
    }


    // Register ports with Jack
    if (getChannel() == 1) {
        if (!(ports[0] = jack_port_register(client,
                                            "mono",
                                            JACK_DEFAULT_AUDIO_TYPE,
                                            JackPortIsInput,
                                            0))) {
            throw Exception( __FILE__, __LINE__,
                            "Cannot register input port", "mono");
        }
    } else if (getChannel() == 2) {
        if (!(ports[0] = jack_port_register(client,
                                            "left",
                                            JACK_DEFAULT_AUDIO_TYPE,
                                            JackPortIsInput,
                                            0))) {
            throw Exception( __FILE__, __LINE__,
                            "Cannot register input port", "left");
        }
        if (!(ports[1] = jack_port_register(client,
                                            "right",
                                            JACK_DEFAULT_AUDIO_TYPE,
                                            JackPortIsInput, 0))) {
            throw Exception( __FILE__, __LINE__,
                            "Cannot register input port", "right");
        }
    } else {
        throw Exception( __FILE__, __LINE__,
                        "Invalid number of channels", getChannel());
    }


    // Create a ring buffer for each channel
    /* will take about 1 MB buffer for each channel */
    rb_size = 5 /* number of seconds */
            * jack_get_sample_rate(client) /* eg 48000 */
            * sizeof (jack_default_audio_sample_t); /* eg 4 bytes */
            
    for (c=0; c < getChannel(); c++) {
        rb[c] = jack_ringbuffer_create(rb_size);
        if (!rb[c]) {
            throw Exception( __FILE__, __LINE__,
                            "Failed to create ringbuffer for", "channel", c);
        }
    }


    // Set the callbacks
    jack_on_shutdown(client, JackDspSource::shutdown_callback, (void*)this);
    if (jack_set_process_callback(client,
                                  JackDspSource::process_callback,
                                  (void*)this)) {
        throw Exception( __FILE__, __LINE__, "Failed to set process callback");
    }    

    // Activate client
    if (jack_activate(client)) {
        throw Exception( __FILE__, __LINE__, "Can't activate client");
    }
    
    // Attempt to automatically connect up our input ports to something ?
    if (auto_connect) {
        do_auto_connect();
    }
    
    return true;
}


/*------------------------------------------------------------------------------
 *  Check whether read() would return anything
 *----------------------------------------------------------------------------*/
bool
JackDspSource :: canRead ( unsigned int   sec,
                           unsigned int   usec )    throw ( Exception )
{
    const unsigned int max_wait_time  = sec * 1000000;
    const unsigned int wait_increment = 10000;
    unsigned int       cur_wait       = 0;

    if ( !isOpen() ) {
        return false;
    }

    while (max_wait_time > cur_wait) {
        bool canRead = true;

        for (unsigned int c = 0 ; c < getChannel() ; c++) {
            if (jack_ringbuffer_read_space(rb[c]) <= 0) {
                canRead = false;
            }
        }

        if (canRead) {
            return true;
        }

        cur_wait += wait_increment;
        usleep ( wait_increment );
    }

    usleep( usec );
    for (unsigned int c = 0 ; c < getChannel() ; c++) {
        if (jack_ringbuffer_read_space(rb[c]) <= 0) {
            return false;
        }
    }

    return true;
}


/*------------------------------------------------------------------------------
 *  Read from the audio source
 *----------------------------------------------------------------------------*/
unsigned int
JackDspSource :: read (   void          * buf,
                          unsigned int    len )     throw ( Exception )
{
    jack_nframes_t samples         = len / 2 / getChannel();
    jack_nframes_t samples_read[2] = { 0, 0 };
    short        * output          = (short*) buf;
    unsigned int c, n;

    if ( !isOpen() ) {
        return 0;
    }


    // Ensure the temporary buffer is big enough
    tmp_buffer = (jack_default_audio_sample_t*)realloc(tmp_buffer,
                             samples * sizeof( jack_default_audio_sample_t ) );
    if (!tmp_buffer) {
        throw Exception( __FILE__, __LINE__, "realloc on tmp_buffer failed");
    }

    // We must be sure to fetch as many data on both channels
    int minBytesAvailable = samples * sizeof( jack_default_audio_sample_t );

    for (c=0; c < getChannel(); c++) {
        int readable = jack_ringbuffer_read_space(rb[c]);
        if (readable < minBytesAvailable) {
            minBytesAvailable = readable;
        }
    }

    for (c=0; c < getChannel(); c++) {    
        // Copy frames from ring buffer to temporary buffer
        // and then convert samples to output buffer
        int bytes_read = jack_ringbuffer_read(rb[c],
                                             (char*)tmp_buffer,
                              minBytesAvailable);
        samples_read[c] = bytes_read / sizeof( jack_default_audio_sample_t );
        

        // Convert samples from float to short and put in output buffer
        for(n=0; n<samples_read[c]; n++) {
            int tmp = lrintf(tmp_buffer[n] * 32768.0f);
            if (tmp > SHRT_MAX) {
                output[n*getChannel()+c] = SHRT_MAX;
            } else if (tmp < SHRT_MIN) {
                output[n*getChannel()+c] = SHRT_MIN;
            } else {
                output[n*getChannel()+c] = (short) tmp;
            }
        }
    }

    // Didn't get as many samples as we wanted ?
    if (getChannel() == 2 && samples_read[0] != samples_read[1]) {
        Reporter::reportEvent( 2,
                              "Warning: Read a different number of samples "
                              "for left and right channels");
    }

    // Return the number of bytes put in the output buffer
    return samples_read[0] * 2 * getChannel();
}


/*------------------------------------------------------------------------------
 *  Close the audio source
 *----------------------------------------------------------------------------*/
void
JackDspSource :: close ( void )                  throw ( Exception )
{
    unsigned int i;

    if ( !isOpen() ) {
        return;
    }

    for(i = 0; i < getChannel(); i++) {
        // Close the port for channel
        if ( ports[i] ) {
            jack_port_unregister( client, ports[i] );
            ports[i] = NULL;
        }
        
        // Free up the ring buffer for channel
        if ( rb[i] ) {
            jack_ringbuffer_free( rb[i] );
            rb[i] = NULL;
        }
    }

    /* Leave the jack graph */
    if (client) {
        jack_client_close(client);
        client = NULL;
    }

}


/*------------------------------------------------------------------------------
 *  Callback called by JACK when audio is available
 *
 *  Don't do anything too expensive here
 *      - just shove audio samples in ring buffer
 *----------------------------------------------------------------------------*/
int
JackDspSource :: process_callback( jack_nframes_t nframes, void *arg )
{
    JackDspSource* self     = (JackDspSource*)arg;
    size_t         to_write = sizeof (jack_default_audio_sample_t) * nframes;
    unsigned int   c;
    
    // Wait until it is ready
    if (self->client == NULL) {
        return 0;
    }
    
    /* copy data to ringbuffer; one per channel */
    for (c=0; c < self->getChannel(); c++) {
      /* check space */
        size_t len;
        if (jack_ringbuffer_write_space(self->rb[c]) < to_write) {
            /* buffer is overflowing, skip the incoming data */
            jack_ringbuffer_write_advance(self->rb[c], to_write); 
            /* prevent blocking the ring buffer by updating internal pointers 
             * jack will now not terminate on xruns
             */
            Reporter::reportEvent( 1, "ring buffer full, skipping data");
            /* We do not return error to jack callback handler and keep going */
        } else {
            /* buffer has space, put data into ringbuffer */
            len = jack_ringbuffer_write(self->rb[c], (char *) jack_port_get_buffer(
                    self->ports[c], nframes), to_write);
            if (len != to_write) 
                Reporter::reportEvent( 1, "failed to write to ring buffer (can not happen)");
        }
    }

    // Success
    return 0;
}

/*------------------------------------------------------------------------------
 *  Callback called when 
 *----------------------------------------------------------------------------*/
void
JackDspSource :: shutdown_callback( void *arg )
{
    //JackDspSource* self = (JackDspSource*)arg;

    Reporter::reportEvent( 1, "JackDspSource :: shutdown_callback");
}


#endif // SUPPORT_JACK_DSP

