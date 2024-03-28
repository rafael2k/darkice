/* ============================================================ include files */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

// compile only if configured for Ogg / FLAC
#ifdef HAVE_FLAC_LIB

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <cstring>
#include <cstdlib>

#include "Exception.h"
#include "Util.h"
#include "FlacLibEncoder.h"
#include "CastSink.h"

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
FlacLibEncoder :: init ( unsigned int compression )

{

    this->compression = compression;

    if ( getInBitsPerSample() != 16 ) {
        throw Exception( __FILE__, __LINE__,
                         "only 16 bits per sample supported at the moment",
                         getInBitsPerSample() );
    }

    if ( getInChannel() != 2 ) {
        throw Exception( __FILE__, __LINE__,
                         "only two channels supported at the moment",
                         getInChannel() );
    }

    if ( getOutSampleRate() != getInSampleRate() ) {
        throw Exception( __FILE__, __LINE__,
                         "resampling not supported at the moment");
    }

    if ( compression < 0 || compression > 8 ) {
        throw Exception( __FILE__, __LINE__,
                         "unsupported compression level for the encoder",
                         compression );
    }

    encoderOpen = false;
}


/*------------------------------------------------------------------------------
 *  Start an encoding session
 *----------------------------------------------------------------------------*/
bool
FlacLibEncoder :: open ( void )

{
    if ( isOpen() ) {
        close();
    }

    // open the underlying sink
    if ( !getSink()->open() ) {
        throw Exception( __FILE__, __LINE__,
                         "FLAC lib opening underlying sink error");
    }

    se = FLAC__stream_encoder_new();
    if (!se) {
        throw Exception( __FILE__, __LINE__,
                         "FLAC encoder creation error");
    }
    FLAC__stream_encoder_set_channels(se, getInChannel());
    FLAC__stream_encoder_set_ogg_serial_number(se, rand());
    FLAC__stream_encoder_set_bits_per_sample(se, getInBitsPerSample());
    FLAC__stream_encoder_set_sample_rate(se, getInSampleRate());
    FLAC__stream_encoder_set_compression_level(se, this->compression);

    FLAC__StreamEncoderInitStatus status;
    status = FLAC__stream_encoder_init_ogg_stream(se, NULL,
                   FlacLibEncoder::encoder_cb,
                   NULL, NULL, NULL, this);
    if (status != FLAC__STREAM_ENCODER_INIT_STATUS_OK) {
        throw Exception( __FILE__, __LINE__,
                         "FLAC encoder initialisation failed");
    }

    encoderOpen = true;

    return true;
}

/*------------------------------------------------------------------------------
 * Callback function for the FLAC encoder
 *----------------------------------------------------------------------------*/
FLAC__StreamEncoderWriteStatus
FlacLibEncoder :: encoder_cb (const FLAC__StreamEncoder *encoder,
                              const FLAC__byte buffer[],
                              size_t len,
                              uint32_t samples,
                              uint32_t current_frame,
                              void *flacencoder ) {
    FlacLibEncoder *fle = (FlacLibEncoder*)flacencoder;
    unsigned int written = fle->getSink()->write(buffer, len);
    // Write callback is called twice; once for the page header, once for the
    // page body. When page header is written, samples is 0.
    if (samples != 0) {
        fle->written = written;
    }
    return FLAC__STREAM_ENCODER_WRITE_STATUS_OK;
}

/*------------------------------------------------------------------------------
 *  Write data to the encoder
 *----------------------------------------------------------------------------*/
unsigned int
FlacLibEncoder :: write ( const void   * buf,
                          unsigned int   len )
{
    if ( !isOpen() || len == 0 ) {
        return 0;
    }
    this->written = 0;

    unsigned int   bitsPerSample = getInBitsPerSample();
    unsigned char *b = (unsigned char*)buf;
    const uint32_t samples = len>>1;
    const uint32_t samples_per_channel = samples/getInChannel();
    FLAC__int32 *buffer = new FLAC__int32[samples];

    Util::conv<FLAC__int32>(bitsPerSample, b, len, buffer, isInBigEndian());

    if (!FLAC__stream_encoder_process_interleaved(se, buffer,
                                                  samples_per_channel)) {
        const char *err = FLAC__stream_encoder_get_resolved_state_string(se);
        size_t needed = snprintf(NULL, 0, "FLAC encoder error: %s", err) + 1;
        char *msg = (char *)malloc(needed);
        snprintf(msg, needed, "FLAC encoder error: %s", err);
        throw Exception( __FILE__, __LINE__, msg);
    }

    delete[] buffer;
    return this->written;
}

/*------------------------------------------------------------------------------
 *  Close the encoding session
 *----------------------------------------------------------------------------*/
void
FlacLibEncoder :: close ( void )

{
    if ( isOpen() ) {

        FLAC__stream_encoder_finish(se);
        getSink()->flush();
        FLAC__stream_encoder_delete(se);
        se = NULL;

        encoderOpen = false;

        getSink()->close();
    }
}

/*------------------------------------------------------------------------------
 *  Flush the data from the encoder
 *----------------------------------------------------------------------------*/
void
FlacLibEncoder :: flush ( void )

{
    if ( !isOpen() ) {
        return;
    }

    getSink()->flush();
}

#endif // HAVE_FLAC_LIB
