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

    // open the underlying sink
    if ( !sink->open() ) {
        throw Exception( __FILE__, __LINE__,
                         "lame lib opening underlying sink error");
    }
 
    lameGlobalFlags = lame_init();

    // ugly lame returns -1 in a pointer on allocation errors
    // this is cast to (long int) so that the pointer can be handled
    // on 64 bit systems as well
    if ( !lameGlobalFlags || ((long int)lameGlobalFlags) == -1 ) {
        throw Exception( __FILE__, __LINE__,
                         "lame lib init error",
                         (long int) lameGlobalFlags);
    }

    if ( 0 > lame_set_num_channels( lameGlobalFlags, getInChannel()) ) {
        throw Exception( __FILE__, __LINE__,
                         "lame lib setting channels error",
                         getInChannel() );
    }

    if ( 0 > lame_set_mode( lameGlobalFlags,
                            getOutChannel() == 1 ? MONO : JOINT_STEREO) ) {
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
    
    switch ( getOutBitrateMode() ) {
        
        case cbr: {

            if ( 0 > lame_set_brate( lameGlobalFlags, getOutBitrate()) ) {
                throw Exception( __FILE__, __LINE__,
                                "lame lib setting output bit rate error",
                                getOutBitrate() );
            }
            
            reportEvent( 5,
                         "set lame bit rate",
                         lame_get_brate( lameGlobalFlags));
            
            double  d = (1.0 - getOutQuality()) * 10.0;

            if ( d > 9 ) {
                d = 9;
            }

            int     q = int (d);

            if ( 0 > lame_set_quality( lameGlobalFlags, q) ) {
                throw Exception( __FILE__, __LINE__,
                                "lame lib setting quality error", q);
            }
            
            reportEvent( 5,
                         "set lame quality",
                         lame_get_quality( lameGlobalFlags));
            } break;

        case abr:

            if ( 0 > lame_set_VBR( lameGlobalFlags,vbr_abr)) {
                throw Exception( __FILE__, __LINE__,
                                 "lame lib setting abr error", vbr_abr);
            }
            
            reportEvent( 5,
                         "set lame abr bitrate",
                         lame_get_VBR( lameGlobalFlags));
            
            if ( 0 > lame_set_VBR_mean_bitrate_kbps( lameGlobalFlags,
                                                     getOutBitrate())) {
                throw Exception( __FILE__, __LINE__,
                                 "lame lib setting abr mean bitrate error",
                                 getOutBitrate());
            }
            
            reportEvent( 5,
                         "set lame abr mean bitrate", 
                         lame_get_VBR_mean_bitrate_kbps( lameGlobalFlags));
            break;

        case vbr: {
        
            if ( 0 > lame_set_VBR( lameGlobalFlags, vbr_mtrh)) {
                throw Exception( __FILE__, __LINE__,
                                 "lame lib setting vbr error", vbr_mtrh );
            }
        
            reportEvent( 5,
                         "set lame vbr bitrate",
                         lame_get_VBR( lameGlobalFlags));

            double  d = (1.0 - getOutQuality()) * 10.0;

            if ( d > 9 ) {
                d = 9;
            }

            int     q = int (d);

            if ( 0 > lame_set_VBR_q( lameGlobalFlags, q) ) {
                throw Exception( __FILE__, __LINE__,
                                 "lame lib setting vbr quality error", q);
            }
        
            reportEvent( 5,
                         "set lame vbr quality",
                         lame_get_VBR_q( lameGlobalFlags));
            } break;
    }


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

	if (getReportVerbosity() >= 3) {
 	   lame_print_config( lameGlobalFlags);
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
    if ( !isOpen() || len == 0 ) {
        return 0;
    }

    unsigned int    bitsPerSample = getInBitsPerSample();
    unsigned int    inChannels    = getInChannel();

    unsigned int    sampleSize = (bitsPerSample / 8) * inChannels;
    unsigned char * b = (unsigned char*) buf;
    unsigned int    processed = len - (len % sampleSize);
    unsigned int    nSamples = processed / sampleSize;
    short int     * leftBuffer  = new short int[nSamples];
    short int     * rightBuffer = new short int[nSamples];

    if ( bitsPerSample == 8 ) {
        Util::conv8( b, processed, leftBuffer, rightBuffer, inChannels);
    } else if ( bitsPerSample == 16 ) {
        Util::conv16( b,
                      processed,
                      leftBuffer,
                      rightBuffer,
                      inChannels,
                      isInBigEndian());
    } else {
        delete[] leftBuffer;
        delete[] rightBuffer;
        throw Exception( __FILE__, __LINE__,
                        "unsupported number of bits per sample for the encoder",
                         bitsPerSample );
    }

    // data chunk size estimate according to lame documentation
    // NOTE: mp3Size is calculated based on the number of input channels
    //       which may be bigger than need, as output channels can be less
    unsigned int    mp3Size = (unsigned int) (1.25 * nSamples + 7200);
    unsigned char * mp3Buf  = new unsigned char[mp3Size];
    int             ret;

    ret = lame_encode_buffer( lameGlobalFlags,
                              leftBuffer,
                              inChannels == 2 ? rightBuffer : leftBuffer,
                              nSamples,
                              mp3Buf,
                              mp3Size );

    delete[] leftBuffer;
    delete[] rightBuffer;

    if ( ret < 0 ) {
        reportEvent( 3, "lame encoding error", ret);
        delete[] mp3Buf;
        return 0;
    }

    unsigned int    written = sink->write( mp3Buf, ret);
    delete[] mp3Buf;
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
    unsigned char * mp3Buf  = new unsigned char[mp3Size];
    int             ret;

    ret = lame_encode_flush( lameGlobalFlags, mp3Buf, mp3Size );

    unsigned int    written = sink->write( mp3Buf, ret);
    delete[] mp3Buf;

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

        sink->close();
    }
}


#endif // HAVE_LAME_LIB


/*------------------------------------------------------------------------------
 
  $Source$

  $Log$
  Revision 1.20  2006/01/25 22:47:15  darkeye
  added mpeg2 support, thanks to Nicholas J Humfrey

  Revision 1.19  2005/04/13 19:04:55  jbebel
  Distribute lame qualities better, and prevent values greater than 9 which are invalid.

  Revision 1.18  2002/10/19 13:31:46  darkeye
  some cleanup with the open() / close() functions

  Revision 1.17  2002/10/19 12:22:10  darkeye
  return 0 immediately for write() if supplied length is 0

  Revision 1.16  2002/08/04 10:26:06  darkeye
  added additional error checking to make sure that outChannel < inChannel

  Revision 1.15  2002/08/03 12:41:18  darkeye
  added possibility to stream in mono when recording in stereo

  Revision 1.14  2002/07/28 00:11:58  darkeye
  bugfix for the previous fix :)

  Revision 1.13  2002/07/28 00:08:37  darkeye
  bugfix: mp3Buf was deleted too early

  Revision 1.12  2002/05/28 12:35:41  darkeye
  code cleanup: compiles under gcc-c++ 3.1, using -pedantic option

  Revision 1.11  2002/04/13 11:26:00  darkeye
  added cbr, abr and vbr setting feature with encoding quality

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

