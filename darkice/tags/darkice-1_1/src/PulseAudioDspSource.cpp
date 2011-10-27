/*------------------------------------------------------------------------------

   Copyright (c) 2000-2007 Tyrell Corporation. All rights reserved.

   Copyright (c) 2004
   LS Informationstechnik (LIKE)
   University of Erlangen Nuremberg
   All rights reserved.

   Tyrell DarkIce

   File     : PulseAudioDspSource.cpp
   Version  : $Revision: 461 $
   Author   : $Author: rafael@riseup.net $
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

/* ============================================================ include files */

#include "AudioSource.h"

// compile only if configured for PULSEAUDIO
#ifdef SUPPORT_PULSEAUDIO_DSP

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Util.h"
#include "Exception.h"
#include "PulseAudioDspSource.h"


/* ===================================================  local data structures */


/* ================================================  local constants & macros */

/*------------------------------------------------------------------------------
 *  File identity
 *----------------------------------------------------------------------------*/
static const char fileid[] = "$Id: PulseAudioDspSource.cpp 461 2009-12-01 12:57:31Z rafael@riseup.net $";


/* ===============================================  local function prototypes */


/* =============================================================  module code */


/*------------------------------------------------------------------------------
 *  Initialize the object
 *----------------------------------------------------------------------------*/
void
PulseAudioDspSource :: init (  const char      * paSourceName )    throw ( Exception )
{

    if (paSourceName == NULL)
    {
        throw Exception( __FILE__, __LINE__, "no paSourceName specified");
    }
    Reporter::reportEvent( 1, "Using PulseAudio source: ", paSourceName);
    Util::strEq( paSourceName , "default" );
    if (Util::strEq( paSourceName , "default" ))
    {
        sourceName = NULL;
    }
    else
    {
        sourceName = Util::strDup( paSourceName);
    }
    ss.channels = getChannel();
    ss.rate = getSampleRate();
    
    //Supported for some bits per sample, both Big and Little endian
    if (isBigEndian())
    {
       switch (getBitsPerSample())
       {
         case 8: 
                 ss.format = PA_SAMPLE_U8;
                 break;
         case 16:
                 ss.format = PA_SAMPLE_S16BE;
                 break;
         case 24:
                 ss.format = PA_SAMPLE_S24BE;
                 break;
         case 32:
                 ss.format = PA_SAMPLE_S32BE;
                 break;
         default:
                 ss.format = PA_SAMPLE_INVALID;
       }
    }
    else 
    {
       switch (getBitsPerSample())
       {
         case 8: 
                 ss.format = PA_SAMPLE_U8;
                 break;
         case 16:
                 ss.format = PA_SAMPLE_S16LE;
                 break;
         case 24:
                 ss.format = PA_SAMPLE_S24LE;
                 break;
         case 32:
                 ss.format = PA_SAMPLE_S32LE;
                 break;
         default:
                 ss.format = PA_SAMPLE_INVALID;
       }
    }
}


/*------------------------------------------------------------------------------
 *  De-initialize the object
 *----------------------------------------------------------------------------*/
void
PulseAudioDspSource :: strip ( void )                      throw ( Exception )
{
    if ( isOpen() ) {
        close();
    }

    delete[] sourceName;
}


/*------------------------------------------------------------------------------
 *  Open the audio source
 *----------------------------------------------------------------------------*/
bool
PulseAudioDspSource :: open ( void )                       throw ( Exception )
{
    char         client_name[255];

    //to identify each darkice on pulseaudio server
    snprintf(client_name, 255, "darkice-%d", getpid());

    if (!(s = pa_simple_new(NULL, client_name, PA_STREAM_RECORD, sourceName, "darkice record", &ss, NULL, NULL, &error))) {
        throw Exception( __FILE__, __LINE__, ": pa_simple_new() failed: %s\n", pa_strerror(error));
    }

    return true;
}


/*------------------------------------------------------------------------------
 *  Check wether read() would return anything
 *----------------------------------------------------------------------------*/
bool
PulseAudioDspSource :: canRead ( unsigned int    sec,
                           unsigned int    usec )    throw ( Exception )
{
//this seems to be a problem. 
//to explore in the future
/*    if ( !isOpen() ) {
        return false;
    }
*/

    /*
     * FIXME How to check if it can read?
     */
    return true; // bad!!
}


/*------------------------------------------------------------------------------
 *  Read from the audio source
 *----------------------------------------------------------------------------*/
unsigned int
PulseAudioDspSource :: read (    void          * buf,
                           unsigned int    len )     throw ( Exception )
{
    int ret;

    ret = pa_simple_read(s, buf, len, &error);
    if ( ret < 0) {
        throw Exception(__FILE__, __LINE__, ": pa_simple_read() failed: %s\n", pa_strerror(error));
    }
    return len;
}


/*------------------------------------------------------------------------------
 *  Close the audio source
 *----------------------------------------------------------------------------*/
void
PulseAudioDspSource :: close ( void )                  throw ( Exception )
{
    if ( !isOpen() ) {
        return;
    }

    pa_simple_free(s);
}

#endif // HAVE_PULSEAUDIO_LIB
