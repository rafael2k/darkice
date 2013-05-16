/*------------------------------------------------------------------------------

   Copyright (c) 2000-2007 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : TwoLameLibEncoder.cpp
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

// compile the whole file only if TwoLame support configured in
#ifdef HAVE_TWOLAME_LIB



#include "Exception.h"
#include "Util.h"
#include "TwoLameLibEncoder.h"


/* ===================================================  local data structures */


/* ================================================  local constants & macros */

/*------------------------------------------------------------------------------
 *  File identity
 *----------------------------------------------------------------------------*/
static const char fileid[] = "$Id: TwoLameLibEncoder.cpp 474 2010-05-10 01:18:15Z rafael@riseup.net $";


/* ===============================================  local function prototypes */


/* =============================================================  module code */



/*------------------------------------------------------------------------------
 *  Initialize the object
 *----------------------------------------------------------------------------*/
void
TwoLameLibEncoder :: init ( void )                  throw ( Exception )
{
	this->twolame_opts    = NULL;

	if ( getInBitsPerSample() != 16 ) {
		throw Exception( __FILE__, __LINE__,
						 "specified bits per sample not supported",
						 getInBitsPerSample() );
	}

	if ( getInChannel() != 1 && getInChannel() != 2 ) {
		throw Exception( __FILE__, __LINE__,
				 "unsupported number of input channels for the encoder",
						 getInChannel() );
	}
	if ( getOutChannel() != 1 && getOutChannel() != 2 ) {
		throw Exception( __FILE__, __LINE__,
				"unsupported number of output channels for the encoder",
						 getOutChannel() );
	}
	if ( getInChannel() < getOutChannel() ) {
		throw Exception( __FILE__, __LINE__,
						 "output channels greater then input channels",
						 getOutChannel() );
	}
}


/*------------------------------------------------------------------------------
 *  Open an encoding session
 *----------------------------------------------------------------------------*/
bool
TwoLameLibEncoder :: open ( void )
                                                            throw ( Exception )
{
    if ( isOpen() ) {
        close();
    }

    // open the underlying sink
    if ( !getSink()->open() ) {
        throw Exception( __FILE__, __LINE__,
                         "TwoLAME lib opening underlying sink error");
    }
 
    twolame_opts = ::twolame_init();

    // ugly twolame returns -1 in a pointer on allocation errors
    if ( !twolame_opts  ) {
        throw Exception( __FILE__, __LINE__,
                         "TwoLAME lib init error",
                         (long int) twolame_opts);
    }

    if ( 0 > twolame_set_num_channels( twolame_opts, getInChannel()) ) {
        throw Exception( __FILE__, __LINE__,
                         "TwoLAME lib setting channels error",
                         getInChannel() );
    }

    if ( 0 > twolame_set_mode( twolame_opts,
                            getOutChannel() == 1 ? TWOLAME_MONO : TWOLAME_JOINT_STEREO) ) {
        throw Exception( __FILE__, __LINE__,
                         "TwoLAME lib setting mode error",
                         TWOLAME_JOINT_STEREO );
    }

    if ( 0 > twolame_set_in_samplerate( twolame_opts, getInSampleRate()) ) {
        throw Exception( __FILE__, __LINE__,
                         "TwoLAME lib setting input sample rate error",
                         getInSampleRate() );
    }

    if ( 0 > twolame_set_out_samplerate( twolame_opts, getOutSampleRate()) ) {
        throw Exception( __FILE__, __LINE__,
                         "TwoLAME lib setting output sample rate error",
                         getOutSampleRate() );
    }

    switch ( getOutBitrateMode() ) {
        
        case cbr: {

            if ( 0 > twolame_set_brate( twolame_opts, getOutBitrate()) ) {
                throw Exception( __FILE__, __LINE__,
                                "TwoLAME lib setting output bit rate error",
                                getOutBitrate() );
            }
        } break;
            
		default: {
		   throw Exception( __FILE__, __LINE__,
							"Unsupported bitrate mode." );
		}
    }


    // let TwoLAME init its own params based on our settings
    if ( 0 > twolame_init_params( twolame_opts) ) {
        throw Exception( __FILE__, __LINE__,
                         "TwoLAME lib initializing params error" );
    }

	// Information about TwoLame's setup
	if (getReportVerbosity() >= 3) {
    	twolame_print_config( twolame_opts);
	}
	
    return true;
}


/*------------------------------------------------------------------------------
 *  Write data to the encoder
 *----------------------------------------------------------------------------*/
unsigned int
TwoLameLibEncoder :: write (   const void    * buf,
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

    // data chunk size estimate according to TwoLAME documentation
    // NOTE: mp2Size is calculated based on the number of input channels
    //       which may be bigger than need, as output channels can be less
    unsigned int    mp2Size = (unsigned int) (1.25 * nSamples + 7200);
    unsigned char * mp2Buf  = new unsigned char[mp2Size];
    int             ret;

    ret = twolame_encode_buffer( twolame_opts,
                              leftBuffer,
                              inChannels == 2 ? rightBuffer : leftBuffer,
                              nSamples,
                              mp2Buf,
                              mp2Size );

    delete[] leftBuffer;
    delete[] rightBuffer;

    if ( ret < 0 ) {
        reportEvent( 3, "TwoLAME encoding error", ret);
        delete[] mp2Buf;
        return 0;
    }

    unsigned int    written = getSink()->write( mp2Buf, ret);
    delete[] mp2Buf;
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
TwoLameLibEncoder :: flush ( void )
                                                            throw ( Exception )
{
    if ( !isOpen() ) {
        return;
    }

    // data chunk size estimate according to TwoLAME documentation
    unsigned int    mp2Size = 7200;
    unsigned char * mp2Buf  = new unsigned char[mp2Size];
    int             ret;

    ret = twolame_encode_flush( twolame_opts, mp2Buf, mp2Size );

    unsigned int    written = getSink()->write( mp2Buf, ret);
    delete[] mp2Buf;

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
TwoLameLibEncoder :: close ( void )                    throw ( Exception )
{
    if ( isOpen() ) {
        flush();
        twolame_close( &twolame_opts );
        getSink()->close();
    }
}


#endif // HAVE_TWOLAME_LIB

