#ifndef FLAC_LIB_ENCODER_H
#define FLAC_LIB_ENCODER_H

#ifndef __cplusplus
#error This is a C++ include file
#endif


/* ============================================================ include files */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>

#ifdef HAVE_FLAC_LIB
#include <FLAC/stream_encoder.h>
#else
#error configure for Ogg FLAC
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
 *  A class representing the ogg FLAC encoder linked as a shared object or
 *  as a static library.
 *
 *  @author  $Author$
 *  @version $Revision$
 */
class FlacLibEncoder : public AudioEncoder, public virtual Reporter
{
    private:

        /**
         *  Value indicating if the encoding process is going on
         */
        bool                            encoderOpen;

        /**
         * Stream encoder instance
         */
        FLAC__StreamEncoder           * se;

        /**
         * Number of bytes written to the sink
         */
        unsigned int                    written;

        /**
         * Compression level of the encoder
         */
        unsigned int                    compression;

        /**
         *  Initialize the object.
         *
         *  @param the compression level
         *  @exception Exception
         */
        void
        init ( unsigned int );

        /**
         * Encoder write callback function
         */
        static FLAC__StreamEncoderWriteStatus
        encoder_cb (const FLAC__StreamEncoder *encoder,
                    const FLAC__byte buffer[],
                    size_t bytes,
                    uint32_t samples,
                    uint32_t current_frame,
                    void *client_data );

        /**
         *  De-initialize the object.
         *
         *  @exception Exception
         */
        inline void
        strip ( void )
        {
        }


    protected:

        /**
         *  Default constructor. Always throws an Exception.
         *
         *  @exception Exception
         */
        inline
        FlacLibEncoder ( void )
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
         *  @param outChannel number of channels of the output.
         *                    If 0, inChannel is used.
         *  @param compression compression level
         *  @exception Exception
         */
        inline
        FlacLibEncoder (  Sink      * sink,
                            unsigned int    inSampleRate,
                            unsigned int    inBitsPerSample,
                            unsigned int    inChannel,
                            bool            inBigEndian,
                            BitrateMode     outBitrateMode,
                            unsigned int    outBitrate,
                            double          outQuality,
                            unsigned int    outSampleRate = 0,
                            unsigned int    outChannel    = 0,
                            unsigned int    compression = 0 )

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
            init( compression );
        }

        /**
         *  Constructor.
         *
         *  @param sink the sink to send encoded output to
         *  @param as get input sample rate, bits per sample and channels *            from this AudioSource.
         *  @param outBitrateMode the bit rate mode of the output.
         *  @param outBitrate bit rate of the output (kbits/sec).
         *  @param outQuality the quality of the stream.
         *  @param outSampleRate sample rate of the output.
         *                       If 0, input sample rate is used.
         *  @param outMaxBitrate maximum output bitrate.
         *                       0 if not used.
         *  @param outChannel number of channels of the output.
         *                    If 0, input channel is used.
         *  @param compression compression level
         *  @exception Exception
         */
        inline
        FlacLibEncoder (    Sink                  * sink,
                            const AudioSource     * as,
                            BitrateMode             outBitrateMode,
                            unsigned int            outBitrate,
                            double                  outQuality,
                            unsigned int            outSampleRate = 0,
                            unsigned int            outChannel    = 0,
                            unsigned int            compression = 0)

                    : AudioEncoder ( sink,
                                     as,
                                     outBitrateMode,
                                     outBitrate,
                                     outQuality,
                                     outSampleRate,
                                     outChannel )
        {
            init( compression );
        }

        /**
         *  Copy constructor.
         *
         *  @param encoder the FlacLibEncoder to copy.
         */
        inline
        FlacLibEncoder (  const FlacLibEncoder &    encoder )

                    : AudioEncoder( encoder )
        {
            if( encoder.isOpen() ) {
                throw Exception(__FILE__, __LINE__, "don't copy open encoders");
            }
            init( compression );
        }

        /**
         *  Destructor.
         *
         *  @exception Exception
         */
        inline virtual
        ~FlacLibEncoder ( void )
        {
            if ( isOpen() ) {
                stop();
            }
            strip();
        }

        /**
         *  Assignment operator.
         *
         *  @param encoder the FlacLibEncoder to assign this to.
         *  @return a reference to this FlacLibEncoder.
         *  @exception Exception
         */
        inline virtual FlacLibEncoder &
        operator= ( const FlacLibEncoder &   encoder )
        {
            if( encoder.isOpen() ) {
                throw Exception(__FILE__, __LINE__, "don't copy open encoders");
            }

            if ( this != &encoder ) {
                strip();
                AudioEncoder::operator=( encoder);
                init( compression );
            }

            return *this;
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
        start ( void )
        {
            return open();
        }

        /**
         *  Stop encoding. Stops the encoding running in the background.
         *
         *  @exception Exception
         */
        inline virtual void
        stop ( void )
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
        open ( void )                               ;

        /**
         *  Check if the encoding session is open.
         *
         *  @return true if the encoding session is open, false otherwise.
         */

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
                       unsigned int    usec )
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
        write (        const void   * buf,
                       unsigned int   len )        ;

        /**
         *  Flush all data that was written to the encoder to the underlying
         *  connection.
         *
         *  @exception Exception
         */
        virtual void
        flush ( void )                              ;

        /**
         *  Close the encoding session.
         *
         *  @exception Exception
         */
        virtual void
        close ( void )                              ;
};


/* ================================================= external data structures */


/* ====================================================== function prototypes */



#endif  /* FLAC_LIB_ENCODER_H */
