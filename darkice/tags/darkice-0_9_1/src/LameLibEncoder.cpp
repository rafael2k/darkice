/*------------------------------------------------------------------------------

   Copyright (c) 2000 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : LameLibEncoder.cpp
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

/* ============================================================ include files */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

// compile the whole file only if lame support configured in
#ifdef HAVE_LAME_LIB


#include "Exception.h"
#include "Util.h"
#include "LameLibEncoder.h"


/* ===================================================  local data structures */


/* ================================================  local constants & macros */

/*------------------------------------------------------------------------------
 *  File identity
 *----------------------------------------------------------------------------*/
static const char fileid[] = "$Id$";


/* ===============================================  local function prototypes */


/* =============================================================  module code */

/*------------------------------------------------------------------------------
 *  Open an encoding session
 *----------------------------------------------------------------------------*/
bool
LameLibEncoder :: open ( void )
                                                            throw ( Exception )
{
    if ( isOpen() ) {
        close();
    }

    lameGlobalFlags = lame_init();

    // ugly lame returns -1 in a pointer on allocation errors
    if ( !lameGlobalFlags || ((int)lameGlobalFlags) == -1 ) {
        throw Exception( __FILE__, __LINE__,
                         "lame lib init error",
                         (int) lameGlobalFlags);
    }

    if ( 0 > lame_set_num_channels( lameGlobalFlags, getInChannel()) ) {
        throw Exception( __FILE__, __LINE__,
                         "lame lib setting channels error",
                         getInChannel() );
    }

    if ( 0 > lame_set_mode( lameGlobalFlags,
                            getInChannel() == 1 ? MONO : JOINT_STEREO) ) {
        throw Exception( __FILE__, __LINE__,
                         "lame lib setting mode error",
                         JOINT_STEREO );
    }

    reportEvent( 5, "set lame mode", lame_get_mode( lameGlobalFlags));
    
    reportEvent( 5,
                 "set lame channels",
                 lame_get_num_channels( lameGlobalFlags));
    
    if ( 0 > lame_set_in_samplerate( lameGlobalFlags, getInSampleRate()) ) {
        throw Exception( __FILE__, __LINE__,
                         "lame lib setting input sample rate error",
                         getInSampleRate() );
    }

    reportEvent( 5,
                 "set lame in sample rate",
                 lame_get_in_samplerate( lameGlobalFlags));
    
    if ( 0 > lame_set_out_samplerate( lameGlobalFlags, getOutSampleRate()) ) {
        throw Exception( __FILE__, __LINE__,
                         "lame lib setting output sample rate error",
                         getOutSampleRate() );
    }

    reportEvent( 5,
                 "set lame out sample rate",
                 lame_get_out_samplerate( lameGlobalFlags));
    
    if ( 0 > lame_set_brate( lameGlobalFlags, getOutBitrate()) ) {
        throw Exception( __FILE__, __LINE__,
                         "lame lib setting output bit rate error",
                         getOutBitrate() );
    }

    reportEvent( 5, "set lame bit rate", lame_get_brate( lameGlobalFlags));
    
    if ( 0 > lame_set_lowpassfreq( lameGlobalFlags, lowpass) ) {
        throw Exception( __FILE__, __LINE__,
                         "lame lib setting lowpass frequency error",
                         lowpass );
    }

    reportEvent( 5,
                 "set lame lowpass frequency",
                 lame_get_lowpassfreq( lameGlobalFlags));
    
    if ( 0 > lame_set_highpassfreq( lameGlobalFlags, highpass) ) {
        throw Exception( __FILE__, __LINE__,
                         "lame lib setting highpass frequency error",
                         lowpass );
    }

    reportEvent( 5,
                 "set lame highpass frequency",
                 lame_get_highpassfreq( lameGlobalFlags));

    // not configurable lame settings

    if ( 0 > lame_set_quality( lameGlobalFlags, 2) ) {
        throw Exception( __FILE__, __LINE__,
                         "lame lib setting quality error",
                         2 );
    }

    reportEvent( 5, "set lame quality", lame_get_quality( lameGlobalFlags));
    
    if ( 0 > lame_set_exp_nspsytune( lameGlobalFlags, 1) ) {
        throw Exception( __FILE__, __LINE__,
                         "lame lib setting  psycho acoustic model error");
    }

    reportEvent( 5,
                 "set lame psycho acoustic model",
                 lame_get_exp_nspsytune( lameGlobalFlags));
    
    if ( 0 > lame_set_error_protection( lameGlobalFlags, 1) ) {
        throw Exception( __FILE__, __LINE__,
                         "lame lib setting error protection error",
                         1 );
    }

    reportEvent( 5,
                 "set lame error protection",
                 lame_get_error_protection( lameGlobalFlags));

    // let lame init its own params based on our settings
    if ( 0 > lame_init_params( lameGlobalFlags) ) {
        throw Exception( __FILE__, __LINE__,
                         "lame lib initializing params error" );
    }

    lame_print_config( lameGlobalFlags);

    // open the underlying sink
    if ( !sink->open() ) {
        throw Exception( __FILE__, __LINE__,
                         "lame lib opening underlying sink error");
    }
 
    return true;
}


/*------------------------------------------------------------------------------
 *  Write data to the encoder
 *----------------------------------------------------------------------------*/
unsigned int
LameLibEncoder :: write (   const void    * buf,
                            unsigned int    len )           throw ( Exception )
{
    if ( !isOpen() ) {
        return 0;
    }

    unsigned int    bitsPerSample = getInBitsPerSample();
    unsigned int    channels      = getInChannel();

    if ( channels != 1 && channels != 2 ) {
        throw Exception( __FILE__, __LINE__,
                         "unsupported number of channels for the encoder",
                         channels );
    }
 
    unsigned int    sampleSize = (bitsPerSample / 8) * channels;
    unsigned char * b = (unsigned char*) buf;
    unsigned int    processed = len - (len % sampleSize);
    unsigned int    nSamples = processed / sampleSize;
    short int       leftBuffer[nSamples];
    short int       rightBuffer[nSamples];

    if ( bitsPerSample == 8 ) {
        Util::conv8( b, processed, leftBuffer, rightBuffer, channels);
    } else if ( bitsPerSample == 16 ) {
        Util::conv16( b,
                      processed,
                      leftBuffer,
                      rightBuffer,
                      channels,
                      isInBigEndian());
    } else {
        throw Exception( __FILE__, __LINE__,
                        "unsupported number of bits per sample for the encoder",
                         bitsPerSample );
    }

    // data chunk size estimate according to lame documentation
    unsigned int    mp3Size = (unsigned int) (1.25 * nSamples + 7200);
    unsigned char   mp3Buf[mp3Size];
    int             ret;

    ret = lame_encode_buffer( lameGlobalFlags,
                              leftBuffer,
                              channels == 2 ? rightBuffer : leftBuffer,
                              nSamples,
                              mp3Buf,
                              mp3Size );
    
    if ( ret < 0 ) {
        reportEvent( 3, "lame encoding error", ret);
        return 0;
    }

    unsigned int    written = sink->write( mp3Buf, ret);
    // just let go data that could not be written
    if ( written < (unsigned int) ret ) {
        reportEvent( 2,
                     "couldn't write all from encoder to underlying sink",
                     ret - written);
    }

    return processed;
}


/*------------------------------------------------------------------------------
 *  Flush the data from the encoder
 *----------------------------------------------------------------------------*/
void
LameLibEncoder :: flush ( void )
                                                            throw ( Exception )
{
    if ( !isOpen() ) {
        return;
    }

    // data chunk size estimate according to lame documentation
    unsigned int    mp3Size = 7200;
    unsigned char   mp3Buf[mp3Size];
    int             ret;

    ret = lame_encode_flush( lameGlobalFlags, mp3Buf, mp3Size );

    unsigned int    written = sink->write( mp3Buf, ret);
    // just let go data that could not be written
    if ( written < (unsigned int) ret ) {
        reportEvent( 2,
                     "couldn't write all from encoder to underlying sink",
                     ret - written);
    }

    sink->flush();
}


/*------------------------------------------------------------------------------
 *  Close the encoding session
 *----------------------------------------------------------------------------*/
void
LameLibEncoder :: close ( void )                    throw ( Exception )
{
    if ( isOpen() ) {
        flush();
        lame_close( lameGlobalFlags);
        lameGlobalFlags = 0;
    }
}


#endif // HAVE_LAME_LIB


/*------------------------------------------------------------------------------
 
  $Source$

  $Log$
  Revision 1.10  2002/03/28 16:38:37  darkeye
  moved functions conv8() and conv16() to class Util

  Revision 1.9  2001/10/20 10:56:45  darkeye
  added possibility to disable highpass and lowpass filters for lame

  Revision 1.8  2001/10/19 12:39:42  darkeye
  created configure options to compile with or without lame / Ogg Vorbis

  Revision 1.7  2001/09/18 14:57:19  darkeye
  finalized Solaris port

  Revision 1.6  2001/09/15 11:35:08  darkeye
  minor fixes

  Revision 1.5  2001/09/02 09:54:12  darkeye
  fixed typos in CVS substition keywords

  Revision 1.4  2001/08/31 20:09:05  darkeye
  added funcitons conv8() and conv16()

  Revision 1.3  2001/08/30 17:25:56  darkeye
  renamed configure.h to config.h

  Revision 1.2  2001/08/29 21:06:16  darkeye
  added real support for 8 / 16 bit mono / stereo input
  (8 bit input still has to be spread on 16 bit words)

  Revision 1.1  2001/08/26 20:44:30  darkeye
  removed external command-line encoder support
  replaced it with a shared-object support for lame with the possibility
  of static linkage


  
------------------------------------------------------------------------------*/

