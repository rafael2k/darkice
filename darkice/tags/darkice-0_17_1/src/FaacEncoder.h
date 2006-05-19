/*------------------------------------------------------------------------------

   Copyright (c) 2005 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : FaacEncoder.h
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
#ifndef AAC_ENCODER_H
#define AAC_ENCODER_H

#ifndef __cplusplus
#error This is a C++ include file
#endif


/* ============================================================ include files */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_FAAC_LIB
#include <faac.h>
#else
#error configure with faac
#endif


#include "Ref.h"
#include "Exception.h"
#include "Reporter.h"
#include "AudioEncoder.h"
#include "Sink.h"


/* ================================================================ constants */


/* =================================================================== macros */


/* =============================================================== data types */

/**
 *  A class representing faac AAC encoder.
 *
 *  @author  $Author$
 *  @version $Revision$
 */
class FaacEncoder : public AudioEncoder, public virtual Reporter
{
    private:

        /**
         *  A flag to indicate if the encoding session is open.
         */
        bool                        faacOpen;

        /**
         *  The handle to the AAC encoder instance.
         */
        faacEncHandle               encoderHandle;

        /**
         *  The maximum number of input samples to supply to the encoder.
         */
        unsigned long               inputSamples;

        /**
         *  The maximum number of output bytes the encoder returns in one call.
         */
        unsigned long               maxOutputBytes;

        /**
         *  Lowpass filter. Sound frequency in Hz, from where up the
         *  input is cut.
         */
        int                             lowpass;

        /**
         *  The Sink to dump mp3 data to
         */
        Ref<Sink>                   sink;

        /**
         *  Initialize the object.
         *
         *  @param sink the sink to send mp3 output to
         *  @param lowpass frequency threshold for the lowpass filter.
         *                 Input above this frequency is cut.
         *                 If 0, faac's default values are used,
         *                 which depends on the out sample rate.
         *  @exception Exception
         */
        inline void
        init ( Sink           * sink,
               int              lowpass)                throw (Exception)
        {
            this->faacOpen        = false;
            this->sink            = sink;
            this->lowpass         = lowpass;

            if ( getInBitsPerSample() != 16 && getInBitsPerSample() != 8 ) {
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
            if ( getInChannel() != getOutChannel() ) {
                throw Exception( __FILE__, __LINE__,
                             "input channels and output channels do not match");
            }
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
        FaacEncoder ( void )                         throw ( Exception )
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
         *                 If 0, faac's default values are used,
         *                 which depends on the out sample rate.
         *  @exception Exception
         */
        inline
        FaacEncoder (   Sink          * sink,
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
            
                    : AudioEncoder ( inSampleRate,
                                     inBitsPerSample,
                                     inChannel,
                                     inBigEndian,
                                     outBitrateMode,
                                     outBitrate,
                                     outQuality,
                                     outSampleRate,
                                     outChannel )
        {
            init( sink, lowpass);
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
         *                 If 0, faac's default values are used,
         *                 which depends on the out sample rate.
         *  @exception Exception
         */
        inline
        FaacEncoder (   Sink                  * sink,
                        const AudioSource     * as,
                        BitrateMode             outBitrateMode,
                        unsigned int            outBitrate,
                        double                  outQuality,
                        unsigned int            outSampleRate = 0,
                        unsigned int            outChannel    = 0,
                        int                     lowpass       = 0)
                                                            throw ( Exception )
            
                    : AudioEncoder ( as,
                                     outBitrateMode,
                                     outBitrate,
                                     outQuality,
                                     outSampleRate,
                                     outChannel )
        {
            init( sink, lowpass);
        }

        /**
         *  Copy constructor.
         *
         *  @param encoder the FaacEncoder to copy.
         */
        inline
        FaacEncoder (  const FaacEncoder &    encoder )
                                                            throw ( Exception )
                    : AudioEncoder( encoder )
        {
            init( encoder.sink.get(), encoder.lowpass);
        }
         

        /**
         *  Destructor.
         *
         *  @exception Exception
         */
        inline virtual
        ~FaacEncoder ( void )                            throw ( Exception )
        {
            if ( isOpen() ) {
                close();
            }
            strip();
        }

        /**
         *  Assignment operator.
         *
         *  @param encoder the FaacEncoder to assign this to.
         *  @return a reference to this FaacEncoder.
         *  @exception Exception
         */
        inline virtual FaacEncoder &
        operator= ( const FaacEncoder &      encoder )   throw ( Exception )
        {
            if ( this != &encoder ) {
                strip();
                AudioEncoder::operator=( encoder);
                init( encoder.sink.get(), encoder.lowpass);
            }

            return *this;
        }

        /**
         *  Get the version string of the underlying faac library.
         *
         *  @return the version string of the underlying faac library.
         */
        inline const char *
        getFaacVersion( void )
        {
            char      * id;
            char      * copyright;

            faacEncGetVersion(&id, &copyright);
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
            return faacOpen;
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


#endif  /* AAC_ENCODER_H */


/*------------------------------------------------------------------------------
 
  $Source$

  $Log$
  Revision 1.3  2006/01/27 08:51:23  darkeye
  fixed issue of double colons at the end of the declaration

  Revision 1.2  2005/04/16 22:19:20  darkeye
  changed remaining typos

  Revision 1.1  2005/04/16 21:57:34  darkeye
  added AAC support through the faac codec, http://www.audiocoding.com/


  
------------------------------------------------------------------------------*/

