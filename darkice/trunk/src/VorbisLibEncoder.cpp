/*------------------------------------------------------------------------------

   Copyright (c) 2000-2007 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : VorbisLibEncoder.cpp
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

// compile only if configured for Ogg Vorbis
#ifdef HAVE_VORBIS_LIB


#include "Exception.h"
#include "Util.h"
#include "VorbisLibEncoder.h"


/* ===================================================  local data structures */


/* ================================================  local constants & macros */

/*------------------------------------------------------------------------------
 *  File identity
 *----------------------------------------------------------------------------*/
static const char fileid[] = "$Id: VorbisLibEncoder.cpp 474 2010-05-10 01:18:15Z rafael@riseup.net $";


/* ===============================================  local function prototypes */


/* =============================================================  module code */

/*------------------------------------------------------------------------------
 *  Initialize the encoder
 *----------------------------------------------------------------------------*/
void
VorbisLibEncoder :: init ( unsigned int     outMaxBitrate )
                                                            throw ( Exception )
{
    this->outMaxBitrate = outMaxBitrate;

    if ( getInBitsPerSample() != 16 && getInBitsPerSample() != 8 ) {
        throw Exception( __FILE__, __LINE__,
                         "specified bits per sample not supported",
                         getInBitsPerSample() );
    }

    if ( getInChannel() != 1 && getInChannel() != 2 ) {
        throw Exception( __FILE__, __LINE__,
                         "unsupported number of channels for the encoder",
                         getInChannel() );
    }

    if ( getOutSampleRate() == getInSampleRate() ) {
        resampleRatio = 1;
        converter     = 0;
    } else {
        resampleRatio = ( (double) getOutSampleRate() /
                          (double) getInSampleRate() );

        // Determine if we can use linear interpolation.
        // The inverse of the ratio must be a power of two for linear mode to
        // be of sufficient quality.
        
        bool    useLinear = true;
        double  inverse   = 1 / resampleRatio;
        int     integer   = (int) inverse;

        // Check that the inverse of the ratio is an integer
        if( integer == inverse ) {
            while( useLinear && integer ) { // Loop through the bits
                // If the lowest order bit is not the only one set
                if( integer & 1 && integer != 1 ) {
                    // Not a power of two; cannot use linear
                    useLinear = false;
                } else {
                    // Shift all the bits over and try again
                    integer >>= 1;
                }
            }
        } else {
           useLinear = false;
        }

        // If we get here and useLinear is still true, then we have
        // a power of two.
           
        // open the aflibConverter in
        // - high quality
        // - linear or quadratic (non-linear) based on algorithm
        // - not filter interpolation
#ifdef HAVE_SRC_LIB
        int srcError = 0;
        converter = src_new(useLinear == true ? SRC_LINEAR : SRC_SINC_FASTEST,
                            getInChannel(), &srcError);
        if(srcError)
            throw Exception (__FILE__, __LINE__, "libsamplerate error: ", src_strerror (srcError));
#else
        converter = new aflibConverter( true, useLinear, false);
#endif
    }

    encoderOpen = false;
}


/*------------------------------------------------------------------------------
 *  Open an encoding session
 *----------------------------------------------------------------------------*/
bool
VorbisLibEncoder :: open ( void )
                                                            throw ( Exception )
{
    int     ret;

    if ( isOpen() ) {
        close();
    }

    // open the underlying sink
    if ( !getSink()->open() ) {
        throw Exception( __FILE__, __LINE__,
                         "vorbis lib opening underlying sink error");
    }

    vorbis_info_init( &vorbisInfo);

    switch ( getOutBitrateMode() ) {

        case cbr: {
                int     maxBitrate = getOutMaxBitrate() * 1000;
                if ( !maxBitrate ) {
                    maxBitrate = -1;
                }
                if ( (ret = vorbis_encode_init( &vorbisInfo,
                                                getOutChannel(),
                                                getOutSampleRate(),
                                                maxBitrate,
                                                getOutBitrate() * 1000,
                                                -1)) ) {
                    throw Exception( __FILE__, __LINE__,
                                     "vorbis encode init error", ret);
                }
            } break;

        case abr:
            /* set non-managed VBR around the average bitrate */
            ret = vorbis_encode_setup_managed( &vorbisInfo,
                                               getOutChannel(),
                                               getOutSampleRate(),
                                               -1,
                                               getOutBitrate() * 1000,
                                               -1 )
               || vorbis_encode_ctl( &vorbisInfo, OV_ECTL_RATEMANAGE_SET, NULL)
               || vorbis_encode_setup_init( &vorbisInfo);
            if ( ret ) {
                throw Exception( __FILE__, __LINE__,
                                 "vorbis encode init error", ret);
            }
            break;

        case vbr:
            if ( (ret = vorbis_encode_init_vbr( &vorbisInfo,
                                                getOutChannel(),
                                                getOutSampleRate(),
                                                getOutQuality() )) ) {
                throw Exception( __FILE__, __LINE__,
                                 "vorbis encode init error", ret);
            }
            break;
    }

    if ( (ret = vorbis_analysis_init( &vorbisDspState, &vorbisInfo)) ) {
        throw Exception( __FILE__, __LINE__, "vorbis analysis init error", ret);
    }

    if ( (ret = vorbis_block_init( &vorbisDspState, &vorbisBlock)) ) {
        throw Exception( __FILE__, __LINE__, "vorbis block init error", ret);
    }

    if ( (ret = ogg_stream_init( &oggStreamState, 0)) ) {
        throw Exception( __FILE__, __LINE__, "ogg stream init error", ret);
    }

    // create an empty vorbis_comment structure
    vorbis_comment_init( &vorbisComment);
    /* FIXME: removed title metadata when the sink type was changed from
     *        CastSink to the more generic Sink.
     *        make sure to add metadata somehow
    // Add comment to vorbis headers to show title in players
    // stupid cast to (char*) because of stupid vorbis API
    if ( getSink()->getName() ) {
        vorbis_comment_add_tag(&vorbisComment,
                               "TITLE",
                               (char*) getSink()->getName());
    }
    */

    // create the vorbis stream headers and send them to the underlying sink
    ogg_packet      header;
    ogg_packet      commentHeader;
    ogg_packet      codeHeader;

    if ( (ret = vorbis_analysis_headerout( &vorbisDspState,
                                           &vorbisComment,
                                           &header,
                                           &commentHeader,
                                           &codeHeader )) ) {
        throw Exception( __FILE__, __LINE__, "vorbis header init error", ret);
    }

    ogg_stream_packetin( &oggStreamState, &header);
    ogg_stream_packetin( &oggStreamState, &commentHeader);
    ogg_stream_packetin( &oggStreamState, &codeHeader);

    ogg_page        oggPage;
    while ( ogg_stream_flush( &oggStreamState, &oggPage) ) {
        getSink()->write( oggPage.header, oggPage.header_len);
        getSink()->write( oggPage.body, oggPage.body_len);
    }

    vorbis_comment_clear( &vorbisComment );

    // initialize the resampling coverter if needed
    if ( converter ) {
#ifdef HAVE_SRC_LIB
        converterData.input_frames   = 4096/((getInBitsPerSample() / 8) * getInChannel());
        converterData.data_in        = new float[converterData.input_frames*getInChannel()];
        converterData.output_frames  = (int) (converterData.input_frames * resampleRatio + 1);
        converterData.data_out       = new float[getInChannel() * converterData.output_frames];
        converterData.src_ratio      = resampleRatio;
        converterData.end_of_input   = 0;
#else
        converter->initialize( resampleRatio, getInChannel());
#endif
    }

    encoderOpen = true;

    return true;
}


/*------------------------------------------------------------------------------
 *  Write data to the encoder
 *----------------------------------------------------------------------------*/
unsigned int
VorbisLibEncoder :: write ( const void    * buf,
                            unsigned int    len )           throw ( Exception )
{
    if ( !isOpen() || len == 0 ) {
        return 0;
    }

    unsigned int    channels      = getInChannel();
    unsigned int    bitsPerSample = getInBitsPerSample();
    unsigned int    sampleSize = (bitsPerSample / 8) * channels;

    unsigned int i;

    if ( getInChannel() == 2 && getOutChannel() == 1 ) {
        for ( i = 0; i < len/sampleSize; i++) {
            if ( bitsPerSample == 8 ) {
                char          * buf8 = (char *) buf;
                unsigned int    ix   = sampleSize * i;
		unsigned int    iix  = ix;
                buf8[i] = (buf8[ix] + buf8[++iix]) / 2;
            }
            if ( bitsPerSample == 16 ) {
                short         * buf16 = (short *) buf;
                unsigned int    ix    = (bitsPerSample >> 3) * i;
		unsigned int    iix   = ix;
                buf16[i] = (buf16[ix] + buf16[++iix]) / 2;
            }
        }
        len      >>= 1;
        channels   = 1;
    }

    sampleSize = (bitsPerSample / 8) * channels;
    unsigned char * b = (unsigned char*) buf;
    unsigned int    processed = len - (len % sampleSize);
    unsigned int    nSamples = processed / sampleSize;
    float        ** vorbisBuffer;


    // convert the byte-based raw input into a short buffer
    // with channels still interleaved
    unsigned int    totalSamples = nSamples * channels;
    short int     * shortBuffer  = new short int[totalSamples];


    Util::conv( bitsPerSample, b, processed, shortBuffer, isInBigEndian());

    if ( converter ) {
        // resample if needed
        int         inCount  = nSamples;
        int         outCount = (int) (inCount * resampleRatio);
        short int * resampledBuffer = new short int[(outCount+1)* channels];
        int         converted;
#ifdef HAVE_SRC_LIB
        converterData.input_frames   = nSamples;
        src_short_to_float_array (shortBuffer, converterData.data_in, totalSamples);
        int srcError = src_process (converter, &converterData);
        if (srcError)
             throw Exception (__FILE__, __LINE__, "libsamplerate error: ", src_strerror (srcError));
        converted = converterData.output_frames_gen;

        src_float_to_short_array(converterData.data_out, resampledBuffer, converted*channels);

#else
        converted = converter->resample( inCount,
                                         outCount,
                                         shortBuffer,
                                         resampledBuffer );
#endif

        vorbisBuffer = vorbis_analysis_buffer( &vorbisDspState,
                                               converted);
        Util::conv( resampledBuffer,
                    converted * channels,
                    vorbisBuffer,
                    channels);
        delete[] resampledBuffer;

        vorbis_analysis_wrote( &vorbisDspState, converted);

    } else {

        vorbisBuffer = vorbis_analysis_buffer( &vorbisDspState, nSamples);
        Util::conv( shortBuffer, totalSamples, vorbisBuffer, channels);
        vorbis_analysis_wrote( &vorbisDspState, nSamples);
    }

    delete[] shortBuffer;
    vorbisBlocksOut();

    return processed;
}


/*------------------------------------------------------------------------------
 *  Flush the data from the encoder
 *----------------------------------------------------------------------------*/
void
VorbisLibEncoder :: flush ( void )
                                                            throw ( Exception )
{
    if ( !isOpen() ) {
        return;
    }

    vorbis_analysis_wrote( &vorbisDspState, 0);
    vorbisBlocksOut();
    getSink()->flush();
}


/*------------------------------------------------------------------------------
 *  Send pending Vorbis blocks to the underlying stream
 *----------------------------------------------------------------------------*/
void
VorbisLibEncoder :: vorbisBlocksOut ( void )                throw ( Exception )
{
    while ( 1 == vorbis_analysis_blockout( &vorbisDspState, &vorbisBlock) ) {
        ogg_packet      oggPacket;
        ogg_page        oggPage;

        vorbis_analysis( &vorbisBlock, &oggPacket);
        vorbis_bitrate_addblock( &vorbisBlock);

        while ( vorbis_bitrate_flushpacket( &vorbisDspState, &oggPacket) ) {

            ogg_stream_packetin( &oggStreamState, &oggPacket);

            while ( ogg_stream_pageout( &oggStreamState, &oggPage) ) {
                int    written;
                
                written  = getSink()->write(oggPage.header, oggPage.header_len);
                written += getSink()->write( oggPage.body, oggPage.body_len);

                if ( written < oggPage.header_len + oggPage.body_len ) {
                    // just let go data that could not be written
                    reportEvent( 2,
                           "couldn't write full vorbis data to underlying sink",
                           oggPage.header_len + oggPage.body_len - written);
                }
            }
        }
    }
}


/*------------------------------------------------------------------------------
 *  Close the encoding session
 *----------------------------------------------------------------------------*/
void
VorbisLibEncoder :: close ( void )                    throw ( Exception )
{
    if ( isOpen() ) {
        flush();

        ogg_stream_clear( &oggStreamState);
        vorbis_block_clear( &vorbisBlock);
        vorbis_dsp_clear( &vorbisDspState);
        vorbis_comment_clear( &vorbisComment);
        vorbis_info_clear( &vorbisInfo);

        encoderOpen = false;

        getSink()->close();
    }
}


#endif // HAVE_VORBIS_LIB

