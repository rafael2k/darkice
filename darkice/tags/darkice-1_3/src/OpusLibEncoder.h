/*------------------------------------------------------------------------------

   Copyright (c) 2000-2007 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : OpusLibEncoder.h
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
#ifndef OPUS_LIB_ENCODER_H
#define OPUS_LIB_ENCODER_H

#ifndef __cplusplus
#error This is a C++ include file
#endif


/* ============================================================ include files */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>

#ifdef HAVE_OPUS_LIB
#include <opus/opus.h>
#include <ogg/ogg.h>
#else
#error configure for Ogg Opus
#endif


#include "Ref.h"
#include "Exception.h"
#include "Reporter.h"
#include "AudioEncoder.h"
#include "Sink.h"
#ifdef HAVE_SRC_LIB
#include <samplerate.h>
#else
#include "aflibConverter.h"
#endif

#include <stdio.h>
#include <cstdlib>

/* ================================================================ constants */


/* =================================================================== macros */


/* =============================================================== data types */

/**
 *  A struct for containing the ogg opus header format, sent at the stream
 *  beginning.
 *
 *  @author $Author$
 *  @version $Revision$
 */
struct OpusIdHeader {
    char magic[8];              /* "OpusHead" */
    unsigned char version;      /* Version (currently 1) */
    unsigned char channels;     /* Number of channels */
    unsigned short preskip;     /* Pre-skip */
    unsigned int samplerate;    /* Original samplerate; informational only */
    unsigned short gain;        /* Output gain, stored in Q7.8 in dB */
    unsigned char chanmap;      /* 0 = mono or stereo L/R, 1=vorbis spec order, 2..254=reserved, 255=undef */

    /**
     * Build an Ogg packet from the header.
     * @param packet output pointer, must be freed by caller
     *
     * @return number of bytes in packet.
     */
    inline int buildPacket( unsigned char** packet) throw ( Exception ) {
        int i = 0;
        // FIXME - doesn't support multistream
        unsigned char* out = (unsigned char*)malloc(19);
        if( out == NULL ) {
             throw Exception( __FILE__, __LINE__, "cannot alloc buffer");
        }
        for( ; i < 8; i++) {
            out[i] = magic[i];
        }
        out[i++] = version;
        out[i++] = channels;
        out[i++] = preskip & 0xff;
        out[i++] = (preskip >> 8) & 0xff;
        out[i++] = samplerate & 0xff;
        out[i++] = (samplerate >> 8) & 0xff;
        out[i++] = (samplerate >> 16) & 0xff;
        out[i++] = (samplerate >> 24) & 0xff;
        out[i++] = gain & 0xff;
        out[i++] = (gain >> 8) & 0xff;
        out[i++] = chanmap;
        *packet = out;
        return i;
    }
};

/**
 *  A struct for containing the ogg opus comment header format
 *
 *  @author $Author$
 *  @version $Revision$
 */
struct OpusCommentHeader {
    /**
     * Struct for each tag pair, in the form of a UTF-8
     * string, "TAG=value"
     *
     * @author $Author$
     * @version $Revision$
     */
    struct Tags {
        unsigned int tag_len;   /* For each tag, how long the following tag is */
        char* tag_str;          /* For each tag, UTF-8 encoded string, in tag=value */
    };
    char magic[8];              /* "OpusTags" */
    unsigned int vendor_length; /* Length of the vendor string */
    char* vendor_string;        /* Vendor string -- parsed as utf-8 */
    unsigned int num_tags;      /* Number of tags following */
    Tags* tags;                 /* Pointer to allocated tag array */

    /**
     * Build an Ogg packet from the header.
     * @param packet output pointer, must be freed by caller
     *
     * @return number of bytes in packet.
     */
    inline int buildPacket( unsigned char** packet) throw ( Exception ) {
        int len = 8 + sizeof(unsigned int) * 2 + vendor_length;
        int pos = 0;
        for( unsigned int i = 0; i < num_tags; i++ )
        {
            len += sizeof(unsigned int) + tags[i].tag_len;
        }
        unsigned char* out = (unsigned char*)malloc(len);
        if( out == NULL ) {
             throw Exception( __FILE__, __LINE__, "cannot alloc buffer");
        }
        for( ; pos < 8; pos++) {
            out[pos] = magic[pos];
        }
        out[pos++] = vendor_length & 0xff;
        out[pos++] = (vendor_length >> 8) & 0xff;
        out[pos++] = (vendor_length >> 16) & 0xff;
        out[pos++] = (vendor_length >> 24) & 0xff;
        for( unsigned int i = 0; i < vendor_length; i++ ) {
            out[pos++] = vendor_string[i];
        }
        out[pos++] = num_tags & 0xff;
        out[pos++] = (num_tags >> 8) & 0xff;
        out[pos++] = (num_tags >> 16) & 0xff;
        out[pos++] = (num_tags >> 24) & 0xff;
        for( unsigned int i = 0; i < num_tags; i++ ) {
            out[pos++] = tags[i].tag_len & 0xff;
            out[pos++] = (tags[i].tag_len >> 8) & 0xff;
            out[pos++] = (tags[i].tag_len >> 16) & 0xff;
            out[pos++] = (tags[i].tag_len >> 24) & 0xff;
            for( unsigned int j = 0; j < tags[i].tag_len; j++ ) {
                out[pos++] = tags[i].tag_str[j];
            }
        }
        *packet = out;
        return len;
    }
};

/**
 *  A class representing the ogg opus encoder linked as a shared object or
 *  as a static library.
 *
 *  @author  $Author$
 *  @version $Revision$
 */
class OpusLibEncoder : public AudioEncoder, public virtual Reporter
{
    private:

        /**
         *  Value indicating if the encoding process is going on
         */
        bool                            encoderOpen;

        /**
         *  Ogg Opus library global info
         */
        OpusEncoder*                    opusEncoder;

        /**
         *  Ogg library global stream state
         */
        ogg_stream_state                oggStreamState;

        ogg_int64_t                     oggGranulePosition;
        ogg_int64_t                     oggPacketNumber;

        unsigned char*                  internalBuffer;
        int                             internalBufferLength;
        bool                            reconnectError;

        /**
         *  Maximum bitrate of the output in kbits/sec. If 0, don't care.
         */
        unsigned int                    outMaxBitrate;

        /**
         *  Resample ratio
         */
        double                          resampleRatio;

        /**
         *  sample rate converter object for possible resampling
         */
#ifdef HAVE_SRC_LIB
        SRC_STATE                     * converter;
        SRC_DATA                      converterData;
#else
        aflibConverter                * converter;
#endif

        /**
         *  Initialize the object.
         *
         *  @param the maximum bit rate
         *  @exception Exception
         */
        void
        init ( unsigned int     outMaxBitrate )         throw ( Exception );

        /**
         *  De-initialize the object.
         *
         *  @exception Exception
         */
        inline void
        strip ( void )                                  throw ( Exception )
        {
            if ( converter ) {
#ifdef HAVE_SRC_LIB
                delete [] converterData.data_in;
                delete [] converterData.data_out;
                src_delete (converter);
#else
                delete converter;
#endif
            }
        }

        /**
         *  Send pending Opus blocks to the underlying stream
         */
        void
        opusBlocksOut( int bytes,
                       unsigned char* data,
                       bool eos = false )               throw ( Exception );


    protected:

        /**
         *  Default constructor. Always throws an Exception.
         *
         *  @exception Exception
         */
        inline
        OpusLibEncoder ( void )                         throw ( Exception )
        {
            throw Exception( __FILE__, __LINE__);
        }


    public:

        /**
         *  Constructor.
         *
         *  @param sink the sink to send encoded output to
         *  @param inSampleRate sample rate of the input.
         *  @param inBitsPerSample number of bits per sample of the input.
         *  @param inChannel number of channels  of the input.
         *  @param inBigEndian shows if the input is big or little endian
         *  @param outBitrateMode the bit rate mode of the output.
         *  @param outBitrate bit rate of the output (kbits/sec).
         *  @param outQuality the quality of the stream.
         *  @param outSampleRate sample rate of the output.
         *                       If 0, inSampleRate is used.
         *  @param outMaxBitrate maximum output bitrate.
         *                       0 if not used.
         *  @param outChannel number of channels of the output.
         *                    If 0, inChannel is used.
         *  @exception Exception
         */
        inline
        OpusLibEncoder (  Sink      * sink,
                            unsigned int    inSampleRate,
                            unsigned int    inBitsPerSample,
                            unsigned int    inChannel,
                            bool            inBigEndian,
                            BitrateMode     outBitrateMode,
                            unsigned int    outBitrate,
                            double          outQuality,
                            unsigned int    outSampleRate = 0,
                            unsigned int    outChannel    = 0,
                            unsigned int    outMaxBitrate = 0 )
                                                        throw ( Exception )

                    : AudioEncoder ( sink,
                                     inSampleRate,
                                     inBitsPerSample,
                                     inChannel,
                                     inBigEndian,
                                     outBitrateMode,
                                     outBitrate,
                                     outQuality,
                                     outSampleRate,
                                     outChannel )
        {
            init( outMaxBitrate);
        }

        /**
         *  Constructor.
         *
         *  @param sink the sink to send encoded output to
         *  @param as get input sample rate, bits per sample and channels
         *            from this AudioSource.
         *  @param outBitrateMode the bit rate mode of the output.
         *  @param outBitrate bit rate of the output (kbits/sec).
         *  @param outQuality the quality of the stream.
         *  @param outSampleRate sample rate of the output.
         *                       If 0, input sample rate is used.
         *  @param outMaxBitrate maximum output bitrate.
         *                       0 if not used.
         *  @param outChannel number of channels of the output.
         *                    If 0, input channel is used.
         *  @exception Exception
         */
        inline
        OpusLibEncoder (    Sink                  * sink,
                            const AudioSource     * as,
                            BitrateMode             outBitrateMode,
                            unsigned int            outBitrate,
                            double                  outQuality,
                            unsigned int            outSampleRate = 0,
                            unsigned int            outChannel    = 0,
                            unsigned int            outMaxBitrate = 0 )
                                                            throw ( Exception )

                    : AudioEncoder ( sink,
                                     as,
                                     outBitrateMode,
                                     outBitrate,
                                     outQuality,
                                     outSampleRate,
                                     outChannel )
        {
            init( outMaxBitrate);
        }

        /**
         *  Copy constructor.
         *
         *  @param encoder the OpusLibEncoder to copy.
         */
        inline
        OpusLibEncoder (  const OpusLibEncoder &    encoder )
                                                            throw ( Exception )
                    : AudioEncoder( encoder )
        {
            if( encoder.isOpen() ) {
                throw Exception(__FILE__, __LINE__, "don't copy open encoders");
            }
            init( encoder.getOutMaxBitrate() );
        }

        /**
         *  Destructor.
         *
         *  @exception Exception
         */
        inline virtual
        ~OpusLibEncoder ( void )                         throw ( Exception )
        {
            if ( isOpen() ) {
                close();
            }
            strip();
        }

        /**
         *  Assignment operator.
         *
         *  @param encoder the OpusLibEncoder to assign this to.
         *  @return a reference to this OpusLibEncoder.
         *  @exception Exception
         */
        inline virtual OpusLibEncoder &
        operator= ( const OpusLibEncoder &   encoder )   throw ( Exception )
        {
            if( encoder.isOpen() ) {
                throw Exception(__FILE__, __LINE__, "don't copy open encoders");
            }

            if ( this != &encoder ) {
                strip();
                AudioEncoder::operator=( encoder);
                init( encoder.getOutMaxBitrate() );
            }

            return *this;
        }

        /**
         *  Get the maximum bit rate of the output in kbits/sec,
         *  for fixed / average bitrate encodings.
         *
         *  @return the maximum bit rate of the output, or 0 if not set.
         */
        inline unsigned int
        getOutMaxBitrate ( void ) const        throw ()
        {
            return outMaxBitrate;
        }

        /**
         *  Check whether encoding is in progress.
         *
         *  @return true if encoding is in progress, false otherwise.
         */
        inline virtual bool
        isRunning ( void ) const           throw ()
        {
            return isOpen();
        }

        /**
         *  Start encoding. This function returns as soon as possible,
         *  with encoding started in the background.
         *
         *  @return true if encoding has started, false otherwise.
         *  @exception Exception
         */
        inline virtual bool
        start ( void )                      throw ( Exception )
        {
            return open();
        }

        /**
         *  Stop encoding. Stops the encoding running in the background.
         *
         *  @exception Exception
         */
        inline virtual void
        stop ( void )                       throw ( Exception )
        {
            return close();
        }

        /**
         *  Open an encoding session.
         *
         *  @return true if opening was successfull, false otherwise.
         *  @exception Exception
         */
        virtual bool
        open ( void )                               throw ( Exception );

        /**
         *  Check if the encoding session is open.
         *
         *  @return true if the encoding session is open, false otherwise.
         */
        inline virtual bool
        isOpen ( void ) const                       throw ()
        {
            return encoderOpen;
        }

        /**
         *  Check if the encoder is ready to accept data.
         *
         *  @param sec the maximum seconds to block.
         *  @param usec micro seconds to block after the full seconds.
         *  @return true if the encoder is ready to accept data,
         *          false otherwise.
         *  @exception Exception
         */
        inline virtual bool
        canWrite (     unsigned int    sec,
                       unsigned int    usec )       throw ( Exception )
        {
            if ( !isOpen() ) {
                return false;
            }

            return getSink()->canWrite(sec, usec);
        }

        /**
         *  Write data to the encoder.
         *  Buf is expected to be a sequence of big-endian 16 bit values,
         *  with left and right channels interleaved. Len is the number of
         *  bytes, must be a multiple of 4.
         *
         *  @param buf the data to write.
         *  @param len number of bytes to write from buf.
         *  @return the number of bytes written (may be less than len).
         *  @exception Exception
         */
        virtual unsigned int
        write (        const void    * buf,
                       unsigned int    len )        throw ( Exception );

        /**
         *  Flush all data that was written to the encoder to the underlying
         *  connection.
         *
         *  @exception Exception
         */
        virtual void
        flush ( void )                              throw ( Exception );

        /**
         *  Close the encoding session.
         *
         *  @exception Exception
         */
        virtual void
        close ( void )                              throw ( Exception );
};


/* ================================================= external data structures */


/* ====================================================== function prototypes */



#endif  /* OPUS_LIB_ENCODER_H */

