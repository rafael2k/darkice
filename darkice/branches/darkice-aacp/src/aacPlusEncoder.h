/*------------------------------------------------------------------------------

   Copyright (c) 2000-2007 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : aacPlusEncoder.h
   Version  : $Revision: 0.2 $
   Author   : $Author: tipok $
   Location : $HeadURL$
   
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
#ifndef AACP_ENCODER_H
#define AACP_ENCODER_H

#ifndef __cplusplus
#error This is a C++ include file
#endif


/* ============================================================ include files */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_AACPLUS_LIB
extern "C" {
#include <libaacplus/cfftn.h>
#include <libaacplus/FloatFR.h>
#include <libaacplus/aacenc.h>
#include <libaacplus/resampler.h>

#include <libaacplus/adts.h>

#include <libaacplus/sbr_main.h>
#include <libaacplus/aac_ram.h>
#include <libaacplus/aac_rom.h>
}
#else
#error configure with aacplus
#endif

#include <cassert>
#include <cstring>

#include "Ref.h"
#include "Exception.h"
#include "Reporter.h"
#include "AudioEncoder.h"
#include "Sink.h"


/* ================================================================ constants */


/* =================================================================== macros */


/* =============================================================== data types */

/**
 *  A class representing aacplus AAC+ encoder.
 *
 *  @author  $Author: tipok $
 *  @version $Revision: 1 $
 */

#define CORE_DELAY   (1600)
#define INPUT_DELAY  ((CORE_DELAY)*2 +6*64-2048+1)     /* ((1600 (core codec)*2 (multi rate) + 6*64 (sbr dec delay) - 2048 (sbr enc delay) + magic*/
#define MAX_DS_FILTER_DELAY 16                         /* the additional max resampler filter delay (source fs)*/

#define CORE_INPUT_OFFSET_PS (0)  /* (96-64) makes AAC still some 64 core samples too early wrt SBR ... maybe -32 would be even more correct, but 1024-32 would need additional SBR bitstream delay by one frame */

class aacPlusEncoder : public AudioEncoder, public virtual Reporter
{
    private:
        /**
         *  A flag to indicate if the encoding session is open.
         */
        bool                        aacplusOpen;
        
        /**
         *  The Sink to dump aac+ data to
         */
        Ref<Sink>                   sink;

		float inBuf[(AACENC_BLOCKSIZE*2 + MAX_DS_FILTER_DELAY + INPUT_DELAY)*MAX_CHANNELS];
		char outBuf[(6144/8)*MAX_CHANNELS+ADTS_HEADER_SIZE];
		IIR21_RESAMPLER IIR21_reSampler[MAX_CHANNELS];

		AACENC_CONFIG     config;
		
		int  error;
		int nChannelsAAC, nChannelsSBR;
		unsigned int sampleRateAAC;
		int bitrate;
		int bandwidth;
		
		unsigned int numAncDataBytes;
		unsigned char ancDataBytes[MAX_PAYLOAD_SIZE];
		
		int numSamplesRead;
		int useParametricStereo;
		int coreWriteOffset;
		int coreReadOffset;
		int envReadOffset;
		int writeOffset;
		struct AAC_ENCODER *aacEnc;
		unsigned int inSamples;
		unsigned int writtenSamples;
		
		HANDLE_SBR_ENCODER hEnvEnc;
		sbrConfiguration sbrConfig;

        /**
         *  Initialize the object.
         *
         *  @param sink the sink to send mp3 output to
         *  @exception Exception
         */
        inline void
        init ( Sink           * sink)                throw (Exception)
        {
            this->aacplusOpen        = false;
            this->sink            = sink;
	    
	    /* TODO: if we have float as input, we don't need conversion */
            if ( getInBitsPerSample() != 16 && getInBitsPerSample() != 32 ) {
                throw Exception( __FILE__, __LINE__,
                                 "specified bits per sample not supported",
                                 getInBitsPerSample() );
            }

            if ( getInChannel() != 2 ) {
                throw Exception( __FILE__, __LINE__,
                         "unsupported number of input channels for the encoder",
                                 getInChannel() );
            }
            if ( getOutChannel() != 2 ) {
                throw Exception( __FILE__, __LINE__,
                        "unsupported number of output channels for the encoder",
                                 getOutChannel() );
            }
	    /* TODO: this will be neede when we implement mono aac+ encoding */
            if ( getInChannel() != getOutChannel() ) {
                throw Exception( __FILE__, __LINE__,
                             "input channels and output channels do not match");
            }
			
            bitrate = getOutBitrate() * 1000;
            bandwidth = 0;
            useParametricStereo = 0;
            numAncDataBytes=0;
            coreWriteOffset = 0;
            coreReadOffset = 0;
            envReadOffset = 0;
            writeOffset = INPUT_DELAY*MAX_CHANNELS;
            writtenSamples = 0;
            aacEnc = NULL;
	    hEnvEnc=NULL;
          	
        }

        /**
         *  De-initialize the object.
         *
         *  @exception Exception
         */
        inline void
        strip ( void )                                  throw ( Exception )
        {
        }


    protected:

        /**
         *  Default constructor. Always throws an Exception.
         *
         *  @exception Exception
         */
        inline
        aacPlusEncoder ( void )                         throw ( Exception )
        {
            throw Exception( __FILE__, __LINE__);
        }


    public:

        /**
         *  Constructor.
         *
         *  @param sink the sink to send mp3 output to
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
         *  @param lowpass frequency threshold for the lowpass filter.
         *                 Input above this frequency is cut.
         *                 If 0, aacplus's default values are used,
         *                 which depends on the out sample rate.
         *  @exception Exception
         */
        inline
        aacPlusEncoder (   Sink          * sink,
                        unsigned int    inSampleRate,
                        unsigned int    inBitsPerSample,
                        unsigned int    inChannel,
                        bool            inBigEndian,
                        BitrateMode     outBitrateMode,
                        unsigned int    outBitrate,
                        double          outQuality,
                        unsigned int    outSampleRate = 0,
                        unsigned int    outChannel    = 0,
                        int             lowpass       = 0)
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
            init( sink);
        }

        /**
         *  Constructor.
         *
         *  @param sink the sink to send mp3 output to
         *  @param as get input sample rate, bits per sample and channels
         *            from this AudioSource.
         *  @param outBitrateMode the bit rate mode of the output.
         *  @param outBitrate bit rate of the output (kbits/sec).
         *  @param outQuality the quality of the stream.
         *  @param outSampleRate sample rate of the output.
         *                       If 0, input sample rate is used.
         *  @param outChannel number of channels of the output.
         *                    If 0, input channel is used.
         *  @param lowpass frequency threshold for the lowpass filter.
         *                 Input above this frequency is cut.
         *                 If 0, aacplus's default values are used,
         *                 which depends on the out sample rate.
         *  @exception Exception
         */
        inline
        aacPlusEncoder (   Sink                  * sink,
                        const AudioSource     * as,
                        BitrateMode             outBitrateMode,
                        unsigned int            outBitrate,
                        double                  outQuality,
                        unsigned int            outSampleRate = 0,
                        unsigned int            outChannel    = 0,
                        int                     lowpass       = 0)
                                                            throw ( Exception )

                    : AudioEncoder ( sink,
                    				 as,
                                     outBitrateMode,
                                     outBitrate,
                                     outQuality,
                                     outSampleRate,
                                     outChannel )
        {
            init( sink);
        }

        /**
         *  Copy constructor.
         *
         *  @param encoder the aacPlusEncoder to copy.
         */
        inline
        aacPlusEncoder (  const aacPlusEncoder &    encoder )
                                                            throw ( Exception )
                    : AudioEncoder( encoder )
        {
            init( encoder.sink.get());
        }


        /**
         *  Destructor.
         *
         *  @exception Exception
         */
        inline virtual
        ~aacPlusEncoder ( void )                            throw ( Exception )
        {
            if ( isOpen() ) {
                close();
            }
            strip();
        }

        /**
         *  Assignment operator.
         *
         *  @param encoder the aacPlusEncoder to assign this to.
         *  @return a reference to this aacPlusEncoder.
         *  @exception Exception
         */
        inline virtual aacPlusEncoder &
        operator= ( const aacPlusEncoder &      encoder )   throw ( Exception )
        {
            if ( this != &encoder ) {
                strip();
                AudioEncoder::operator=( encoder);
                init( encoder.sink.get());
            }

            return *this;
        }

        /**
         *  Get the version string of the underlying aacplus library.
         *
         *  @return the version string of the underlying aacplus library.
         */
        inline const char *
        getAacPlusVersion( void )
        {
            char      * id;
            //char      * copyright;

            /* aacplusEncGetVersion(&id, &copyright); */
            return id;
        }

        /**
         *  Check wether encoding is in progress.
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
            return aacplusOpen;
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

            return true;
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


#endif  /* AACP_ENCODER_H */

