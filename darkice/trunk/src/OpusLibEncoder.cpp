/*------------------------------------------------------------------------------

   Copyright (c) 2000-2007 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : OpusLibEncoder.cpp
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

// compile only if configured for Ogg / Opus
#ifdef HAVE_OPUS_LIB

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "Exception.h"
#include "Util.h"
#include "OpusLibEncoder.h"
#include "CastSink.h"
#include <cstring>
#include <cstdlib>


/* ===================================================  local data structures */


/* ================================================  local constants & macros */

/*------------------------------------------------------------------------------
 *  File identity
 *----------------------------------------------------------------------------*/
static const char fileid[] = "$Id$";


/* ===============================================  local function prototypes */


/* =============================================================  module code */

/*------------------------------------------------------------------------------
 *  Initialize the encoder
 *----------------------------------------------------------------------------*/
void
OpusLibEncoder :: init ( unsigned int     outMaxBitrate )
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

    if ( getOutSampleRate() != 48000 ) {
        throw Exception( __FILE__, __LINE__,
                         "unsupported sample rate for this encoder, you should resample your input to 48000Hz",
                         getOutSampleRate() );
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
OpusLibEncoder :: open ( void )
                                                            throw ( Exception )
{
    int     ret;

    if ( isOpen() ) {
        close();
    }

    // open the underlying sink
    if ( !getSink()->open() ) {
        throw Exception( __FILE__, __LINE__,
                         "opus lib opening underlying sink error");
    }

    int bufferSize = (getInBitsPerSample()/8) * getInChannel() * 480;
    internalBuffer = new unsigned char[bufferSize];
    internalBufferLength = 0;
    memset( internalBuffer, 0, bufferSize);

    int err;
    opusEncoder = opus_encoder_create( getOutSampleRate(),
                                       getInChannel(),
                                       OPUS_APPLICATION_AUDIO,
                                       &err);
    if( err != OPUS_OK ) {
        throw Exception( __FILE__, __LINE__,
                         "opus encoder creation error",
                         err);
    }

    opus_encoder_ctl(opusEncoder, OPUS_SET_COMPLEXITY(10));
    opus_encoder_ctl(opusEncoder, OPUS_SET_SIGNAL(OPUS_SIGNAL_MUSIC));

    switch ( getOutBitrateMode() ) {

        case cbr: {
                int     maxBitrate = getOutBitrate() * 1000;
                if ( !maxBitrate ) {
                    maxBitrate = 96000;
                }
                opus_encoder_ctl(opusEncoder, OPUS_SET_BITRATE(maxBitrate));
                opus_encoder_ctl(opusEncoder, OPUS_SET_VBR(0));
            } break;

        case abr: {
                int     maxBitrate = getOutBitrate() * 1000;
                if ( !maxBitrate ) {
                    maxBitrate = 96000;
                }
                /* set non-managed VBR around the average bitrate */
                opus_encoder_ctl(opusEncoder, OPUS_SET_BITRATE(maxBitrate));
                opus_encoder_ctl(opusEncoder, OPUS_SET_VBR(1));
                opus_encoder_ctl(opusEncoder, OPUS_SET_VBR_CONSTRAINT(1));
            } break;
        case vbr:
                int     maxBitrate = getOutBitrate() * 1000;
                if ( !maxBitrate ) {
                    maxBitrate = 96000;
                }
                /* set non-managed VBR around the average bitrate */
                opus_encoder_ctl(opusEncoder, OPUS_SET_BITRATE(maxBitrate));
                opus_encoder_ctl(opusEncoder, OPUS_SET_VBR(1));
                opus_encoder_ctl(opusEncoder, OPUS_SET_VBR_CONSTRAINT(0));
                break;
    }

    if ( (ret = ogg_stream_init( &oggStreamState, 0)) ) {
        throw Exception( __FILE__, __LINE__, "ogg stream init error", ret);
    }

    // First, we need to assemble and send a OggOpus header.
    OpusIdHeader header;
    strncpy(header.magic, "OpusHead", 8);
    header.version = 1;
    header.channels = getOutChannel();
    header.preskip = 0;
    header.samplerate = getInSampleRate();
    header.gain = 0; // technically a fixed-point decimal.
    header.chanmap = 0;

    // And, now we need to send a Opus comment header.
    // Anything after this can be audio.

    char vendor[8] = "darkice";
    char titlestr[7] = "TITLE=";
    OpusCommentHeader::Tags tags[1];
    char name[40];
    CastSink* sink = dynamic_cast<CastSink*>(getSink().get());
    if( sink && sink->getName() ) {
        strncpy(name, (char*)sink->getName(), 39);
        name[39] = 0;
    }
    else {
        strncpy(name, "Darkice Stream", 39);
    }
    tags[0].tag_len = strlen(titlestr) + strlen(name);
    tags[0].tag_str = (char*) malloc( tags[0].tag_len + 1 );
    if( tags[0].tag_str == NULL ) {
        throw Exception( __FILE__, __LINE__, "malloc failed");
    }
    strncpy( tags[0].tag_str, titlestr, tags[0].tag_len);
    strncat( tags[0].tag_str, name, tags[0].tag_len);

    OpusCommentHeader commentHeader;
    strncpy(commentHeader.magic, "OpusTags", 8);
    commentHeader.vendor_length = strlen(vendor);
    commentHeader.vendor_string = vendor;
    commentHeader.num_tags = 1;
    commentHeader.tags = tags;

    ogg_packet      oggHeader;
    ogg_packet      oggCommentHeader;
    memset(&oggHeader, 0, sizeof(oggHeader));
    memset(&oggCommentHeader, 0, sizeof(oggCommentHeader));
    unsigned char* headerData = NULL;
    unsigned char* commentData = NULL;
    int headerLen = 0;
    int commentLen = 0;

    headerLen = header.buildPacket( &headerData);
    commentLen = commentHeader.buildPacket( &commentData);

    oggHeader.packet = headerData;
    oggHeader.bytes = headerLen;
    oggHeader.b_o_s = 1;
    oggHeader.e_o_s = 0;
    oggHeader.granulepos = 0;
    oggHeader.packetno = 0;

    oggCommentHeader.packet = commentData;
    oggCommentHeader.bytes = commentLen;
    oggCommentHeader.b_o_s = 0;
    oggCommentHeader.e_o_s = 0;
    oggCommentHeader.granulepos = 0;
    oggCommentHeader.packetno = 1;

    oggPacketNumber = 2;
    oggGranulePosition = 0;

    ogg_stream_packetin( &oggStreamState, &oggHeader);
    ogg_stream_packetin( &oggStreamState, &oggCommentHeader);

    ogg_page oggPage;
    while ( ogg_stream_flush( &oggStreamState, &oggPage) ) {
        getSink()->write( oggPage.header, oggPage.header_len);
        getSink()->write( oggPage.body, oggPage.body_len);
    }

    free(tags[0].tag_str);
    free(headerData);
    free(commentData);

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
    reconnectError = false;

    return true;
}


/*------------------------------------------------------------------------------
 *  Write data to the encoder
 *----------------------------------------------------------------------------*/
unsigned int
OpusLibEncoder :: write ( const void    * buf,
                          unsigned int    len )             throw ( Exception )
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
    unsigned int    processed = 0;
    unsigned int    bytesToProcess = len - (len % sampleSize);
    unsigned int    totalProcessed = 0;
    unsigned char * b = (unsigned char*) buf;
    unsigned char * tempBuffer = NULL;

    if( internalBufferLength > 0 ) {
        tempBuffer = new unsigned char[len + internalBufferLength];
        memset( tempBuffer, 0, len + internalBufferLength);
        if( !tempBuffer ) {
            throw Exception( __FILE__, __LINE__, "could not allocate temp buffer");
        }
        memcpy( tempBuffer, internalBuffer, internalBufferLength);
        memcpy( tempBuffer+internalBufferLength, buf, len);
        b = tempBuffer;
        bytesToProcess += internalBufferLength;
    }


    while ( bytesToProcess / resampleRatio >= 480 * sampleSize ) {
        unsigned int toProcess = bytesToProcess / (resampleRatio * sampleSize);
        if( toProcess >= 480 / resampleRatio) {
            processed = 480 / resampleRatio;
        }

        int opusBufferSize = (1275*3+7)*channels;
        unsigned char*   opusBuffer = new unsigned char[opusBufferSize];

        // convert the byte-based raw input into a short buffer
        // with channels still interleaved
        unsigned int    totalSamples = processed * channels;
        short int     * shortBuffer  = new short int[totalSamples];

        Util::conv( bitsPerSample, b, processed*sampleSize, shortBuffer, isInBigEndian());

        if ( converter && processed > 0 ) {
            // resample if needed
            int         inCount  = processed;
            int         outCount = 480; //(int) (inCount * resampleRatio);
            short int * resampledBuffer = new short int[(outCount+1)* channels];
            int         converted;
#ifdef HAVE_SRC_LIB
            (void)inCount;
            converterData.input_frames   = processed;
            src_short_to_float_array (shortBuffer, (float *) converterData.data_in, totalSamples);
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
            if( converted != 480) {
                throw Exception( __FILE__, __LINE__, "resampler error: expected 480 samples", converted);
            }
            int encBytes = opus_encode( opusEncoder, resampledBuffer, 480, opusBuffer, opusBufferSize);
            if( encBytes == -1 ) {
                throw Exception( __FILE__, __LINE__, "opus encoder error");
            }
            oggGranulePosition += converted;
            opusBlocksOut( encBytes, opusBuffer);

            delete[] resampledBuffer;

        } else if( processed > 0) {
            memset( opusBuffer, 0, opusBufferSize);
            int encBytes = opus_encode( opusEncoder, shortBuffer, processed, opusBuffer, opusBufferSize);
            if( encBytes == -1 ) {
                throw Exception( __FILE__, __LINE__, "opus encoder error");
            }
            oggGranulePosition += processed;
            opusBlocksOut( encBytes, opusBuffer);

        }
        delete[] shortBuffer;
        delete[] opusBuffer;
        bytesToProcess -= processed * sampleSize;
        totalProcessed += processed * sampleSize;
        b = ((unsigned char*)b) + (processed * sampleSize);
    }

    int newLen = len - (len % sampleSize) + internalBufferLength - totalProcessed;
    newLen -= newLen % sampleSize;

    if( newLen > 0 ) {
        memcpy( internalBuffer, b, newLen);
        totalProcessed += newLen;
        internalBufferLength = newLen;
    } else {
        internalBufferLength = 0;
    }

    if( tempBuffer ) {
        delete[] tempBuffer;
        tempBuffer = NULL;
    }

    return totalProcessed;
}


/*------------------------------------------------------------------------------
 *  Flush the data from the encoder
 *----------------------------------------------------------------------------*/
void
OpusLibEncoder :: flush ( void )
                                                            throw ( Exception )
{
    if ( !isOpen() || reconnectError == true ) {
        return;
    }

    int opusBufferSize = (1275*3+7)*getOutChannel();
    unsigned char * opusBuffer = new unsigned char[opusBufferSize];
    short int * shortBuffer = new short int[480*getInChannel()];

    // Send an empty audio packet along to flush out the stream.
    memset( shortBuffer, 0, 480*getInChannel()*sizeof(*shortBuffer));
    memset( opusBuffer, 0, opusBufferSize);
    int encBytes = opus_encode( opusEncoder, shortBuffer, 480, opusBuffer, opusBufferSize);
    if( encBytes == -1 ) {
        throw Exception( __FILE__, __LINE__, "opus encoder error");
    }
    oggGranulePosition += 480;

    // Send the empty block to the Ogg layer, and mark the
    // EOS flag.  This will trigger any remaining packets to be
    // sent.
    opusBlocksOut( encBytes, opusBuffer, true);
    delete[] opusBuffer;
    delete[] shortBuffer;
    getSink()->flush();
}


/*------------------------------------------------------------------------------
 *  Send pending Opus blocks to the underlying stream
 *----------------------------------------------------------------------------*/
void
OpusLibEncoder :: opusBlocksOut ( int bytes,
                                  unsigned char* data,
                                  bool eos )               throw ( Exception )
{
    ogg_packet      oggPacket;
    ogg_page        oggPage;

    oggPacket.packet = data;
    oggPacket.bytes = bytes;
    oggPacket.b_o_s = 0;
    oggPacket.e_o_s = ( eos ) ? 1 : 0;
    oggPacket.granulepos = oggGranulePosition;
    oggPacket.packetno = oggPacketNumber;
    oggPacketNumber++;

    if( ogg_stream_packetin( &oggStreamState, &oggPacket) == 0) {
        while( ogg_stream_pageout( &oggStreamState, &oggPage) ||
            ( eos && ogg_stream_flush( &oggStreamState, &oggPage) ) ) {
            int    written;

            written  = getSink()->write(oggPage.header, oggPage.header_len);
            written += getSink()->write( oggPage.body, oggPage.body_len);

            if ( written < oggPage.header_len + oggPage.body_len ) {
                reconnectError = true;
                // just let go data that could not be written
                reportEvent( 2,
                       "couldn't write full opus data to underlying sink",
                       oggPage.header_len + oggPage.body_len - written);
            }
        }
    } else {
        throw Exception( __FILE__, __LINE__, "internal ogg error");
    }
}


/*------------------------------------------------------------------------------
 *  Close the encoding session
 *----------------------------------------------------------------------------*/
void
OpusLibEncoder :: close ( void )                    throw ( Exception )
{
    if ( isOpen() ) {
        flush();

        ogg_stream_clear( &oggStreamState);
        opus_encoder_destroy( opusEncoder);
        opusEncoder = NULL;

        encoderOpen = false;
        if (internalBuffer) {
            delete[] internalBuffer;
            internalBuffer = NULL;
        }
        else {
            fprintf(stderr, "Opus internalBuffer is NULL!\n");
        }

        getSink()->close();
    }
}


#endif // HAVE_OPUS_LIB

