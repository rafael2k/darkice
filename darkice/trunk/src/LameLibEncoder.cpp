/*------------------------------------------------------------------------------

   Copyright (c) 2000-2007 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : LameLibEncoder.cpp
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
static const char fileid[] = "$Id: LameLibEncoder.cpp 474 2010-05-10 01:18:15Z rafael@riseup.net $";


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
    if ( !getSink()->open() ) {
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

    unsigned int    written = getSink()->write( mp3Buf, ret);
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

    unsigned int    written = getSink()->write( mp3Buf, ret);
    delete[] mp3Buf;

    // just let go data that could not be written
    if ( written < (unsigned int) ret ) {
        reportEvent( 2,
                     "couldn't write all from encoder to underlying sink",
                     ret - written);
    }

    getSink()->flush();
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

        getSink()->close();
    }
}


#endif // HAVE_LAME_LIB

