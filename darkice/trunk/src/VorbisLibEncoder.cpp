/*------------------------------------------------------------------------------

   Copyright (c) 2000 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : VorbisLibEncoder.cpp
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


#include "Exception.h"
#include "Util.h"
#include "VorbisLibEncoder.h"


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
VorbisLibEncoder :: open ( void )
                                                            throw ( Exception )
{
    int     ret;

    if ( isOpen() ) {
        close();
    }

    vorbis_info_init( &vorbisInfo);
    if ( (ret = vorbis_encode_init( &vorbisInfo,
                                    getInChannel(),
                                    getInSampleRate(),
                                    getOutBitrate(),
                                    getOutBitrate(),
                                    getOutBitrate() )) ) {
        throw Exception( __FILE__, __LINE__, "vorbis encode init error", ret);
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

    // open the underlying sink
    if ( !sink->open() ) {
        throw Exception( __FILE__, __LINE__,
                         "vorbis lib opening underlying sink error");
    }

    // create an empty vorbis_comment structure
    vorbis_comment  vorbisComment;

    vorbis_comment_init( &vorbisComment);

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

    vorbis_comment_clear( &vorbisComment );

    ogg_stream_packetin( &oggStreamState, &header);
    ogg_stream_packetin( &oggStreamState, &commentHeader);
    ogg_stream_packetin( &oggStreamState, &codeHeader);

    ogg_page        oggPage;
    while ( ogg_stream_flush( &oggStreamState, &oggPage) ) {
        sink->write( oggPage.header, oggPage.header_len);
        sink->write( oggPage.body, oggPage.body_len);
    }

    encoderOpen = true;

    return true;
}

/*------------------------------------------------------------------------------
 *  Convert an unsigned char buffer holding 8 bit PCM values with channels
 *  interleaved to two float buffers (one for each channel)
 *----------------------------------------------------------------------------*/
void
VorbisLibEncoder :: conv8 ( unsigned char     * pcmBuffer,
                            unsigned int        lenPcmBuffer,
                            float             * leftBuffer,
                            float             * rightBuffer,
                            unsigned int        channels )
{
    if ( channels == 1 ) {
        unsigned int    i, j;

        for ( i = 0, j = 0; i < lenPcmBuffer; ) {
            short int  value;

            value          = pcmBuffer[i++];
            leftBuffer[j]  = ((float) value) / 128.f;
            ++j;
        }
    } else {
        unsigned int    i, j;

        for ( i = 0, j = 0; i < lenPcmBuffer; ) {
            short int  value;

            value          = pcmBuffer[i++];
            leftBuffer[j]  = ((float) value) / 128.f;
            value          = pcmBuffer[i++];
            rightBuffer[j] = ((float) value) / 128.f;
            ++j;
        }
    }
}


/*------------------------------------------------------------------------------
 *  Convert an unsigned char buffer holding 16 bit PCM values with channels
 *  interleaved to two float buffers (one for each channel)
 *----------------------------------------------------------------------------*/
void
VorbisLibEncoder :: conv16 (    unsigned char     * pcmBuffer,
                                unsigned int        lenPcmBuffer,
                                float             * leftBuffer,
                                float             * rightBuffer,
                                unsigned int        channels )
{
    if ( channels == 1 ) {
        unsigned int    i, j;

        for ( i = 0, j = 0; i < lenPcmBuffer; ) {
            short int   value;

            value           = pcmBuffer[i++];
            value          |= pcmBuffer[i++] << 8;
            leftBuffer[j]   = ((float) value) / 32768.f;
            ++j;
        }
    } else {
        unsigned int    i, j;

        for ( i = 0, j = 0; i < lenPcmBuffer; ) {
            short int   value;

            value           = pcmBuffer[i++];
            value          |= pcmBuffer[i++] << 8;
            leftBuffer[j]   = ((float) value) / 32768.f;
            value           = pcmBuffer[i++];
            value          |= pcmBuffer[i++] << 8;
            rightBuffer[j]  = ((float) value) / 32768.f;
            ++j;
        }
    }
}


/*------------------------------------------------------------------------------
 *  Write data to the encoder
 *----------------------------------------------------------------------------*/
unsigned int
VorbisLibEncoder :: write ( const void    * buf,
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
    float        ** vorbisBuffer;

    vorbisBuffer = vorbis_analysis_buffer( &vorbisDspState, nSamples);

    if ( bitsPerSample == 8 ) {
        conv8( b, processed, vorbisBuffer[0], vorbisBuffer[1], channels);
    } else if ( bitsPerSample == 16 ) {
        conv16( b, processed, vorbisBuffer[0], vorbisBuffer[1], channels);
    } else {
        throw Exception( __FILE__, __LINE__,
                        "unsupported number of bits per sample for the encoder",
                         bitsPerSample );
    }

    vorbis_analysis_wrote( &vorbisDspState, nSamples);
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
    sink->flush();
}


/*------------------------------------------------------------------------------
 *  Send pending Vorbis blocks to the underlying stream
 *----------------------------------------------------------------------------*/
void
VorbisLibEncoder :: vorbisBlocksOut ( void )                throw ()
{
    while ( 1 == vorbis_analysis_blockout( &vorbisDspState, &vorbisBlock) ) {
        ogg_packet      oggPacket;
        ogg_page        oggPage;

        vorbis_analysis( &vorbisBlock, &oggPacket);
        ogg_stream_packetin( &oggStreamState, &oggPacket);

        while ( ogg_stream_pageout( &oggStreamState, &oggPage) ) {
            int    written;
            
            written  = sink->write( oggPage.header, oggPage.header_len);
            written += sink->write( oggPage.body, oggPage.body_len);

            if ( written < oggPage.header_len + oggPage.body_len ) {
                // just let go data that could not be written
                reportEvent( 2,
                           "couldn't write full vorbis data to underlying sink",
                             oggPage.header_len + oggPage.body_len - written);
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
        vorbis_info_clear( &vorbisInfo);

        encoderOpen = false;
    }
}



/*------------------------------------------------------------------------------
 
  $Source$

  $Log$
  Revision 1.2  2001/09/15 11:36:22  darkeye
  added function vorbisBlocksOut(), finalized vorbis support

  Revision 1.1  2001/09/14 19:31:06  darkeye
  added IceCast2 / vorbis support


  
------------------------------------------------------------------------------*/

