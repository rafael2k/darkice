/*------------------------------------------------------------------------------

   Copyright (c) 2000 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : VorbisLibEncoder.h
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
#ifndef VORBIS_LIB_ENCODER_H
#define VORBIS_LIB_ENCODER_H

#ifndef __cplusplus
#error This is a C++ include file
#endif


/* ============================================================ include files */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_VORBIS_LIB
#include <vorbis/vorbisenc.h>
#else
#error configure for Ogg Vorbis
#endif


#include "Ref.h"
#include "Exception.h"
#include "Reporter.h"
#include "AudioEncoder.h"
#include "CastSink.h"
#include "aflibConverter.h"


/* ================================================================ constants */


/* =================================================================== macros */


/* =============================================================== data types */

/**
 *  A class representing the ogg vorbis encoder linked as a shared object or
 *  as a static library.
 *
 *  @author  $Author$
 *  @version $Revision$
 */
class VorbisLibEncoder : public AudioEncoder, public virtual Reporter
{
    private:

        /**
         *  Value indicating if the encoding process is going on
         */
        bool                            encoderOpen;

        /**
         *  Ogg Vorbis library global info
         */
        vorbis_info                     vorbisInfo;

        /**
         *  Ogg Vorbis library global DSP state
         */
        vorbis_dsp_state                vorbisDspState;

        /**
         *  Ogg Vorbis library global block
         */
        vorbis_block                    vorbisBlock;

        /**
         *  Ogg Vorbis library global comment
         */
        vorbis_comment                  vorbisComment;

        /**
         *  Ogg library global stream state
         */
        ogg_stream_state                oggStreamState;

        /**
         *  Maximum bitrate of the output in kbits/sec. If 0, don't care.
         */
        unsigned int                    outMaxBitrate;

        /**
         *  Resample ratio
         */
        double                          resampleRatio;

        /**
         *  aflibConverter object for possible resampling
         */
        aflibConverter                * converter;

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
                delete converter;
            }
        }

        /**
         *  Send pending Vorbis blocks to the underlying stream
         */
        void
        vorbisBlocksOut( void )                         throw ( Exception );


    protected:

        /**
         *  Default constructor. Always throws an Exception.
         *
         *  @exception Exception
         */
        inline
        VorbisLibEncoder ( void )                         throw ( Exception )
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
        VorbisLibEncoder (  CastSink      * sink,
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
        VorbisLibEncoder (  CastSink              * sink,
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
         *  @param encoder the VorbisLibEncoder to copy.
         */
        inline
        VorbisLibEncoder (  const VorbisLibEncoder &    encoder )
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
        ~VorbisLibEncoder ( void )                         throw ( Exception )
        {
            if ( isOpen() ) {
                close();
            }
            strip();
        }

        /**
         *  Assignment operator.
         *
         *  @param encoder the VorbisLibEncoder to assign this to.
         *  @return a reference to this VorbisLibEncoder.
         *  @exception Exception
         */
        inline virtual VorbisLibEncoder &
        operator= ( const VorbisLibEncoder &   encoder )   throw ( Exception )
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



#endif  /* VORBIS_LIB_ENCODER_H */


/*------------------------------------------------------------------------------
 
  $Source$

  $Log$
  Revision 1.9  2003/02/09 13:15:57  darkeye
  added feature for setting the TITLE comment field for vorbis streams

  Revision 1.8  2002/08/20 20:07:36  darkeye
  minor fixes

  Revision 1.7  2002/08/20 19:35:37  darkeye
  added possibility to specify maximum bitrate for Ogg Vorbis streams

  Revision 1.6  2002/07/20 16:37:06  darkeye
  added fault tolerance in case a server connection is dropped

  Revision 1.5  2002/04/13 11:26:00  darkeye
  added cbr, abr and vbr setting feature with encoding quality

  Revision 1.4  2002/03/28 16:47:38  darkeye
  moved functions conv8() and conv16() to class Util (as conv())
  added resampling functionality
  added support for variable bitrates

  Revision 1.3  2001/10/19 12:39:42  darkeye
  created configure options to compile with or without lame / Ogg Vorbis

  Revision 1.2  2001/09/15 11:36:22  darkeye
  added function vorbisBlocksOut(), finalized vorbis support

  Revision 1.1  2001/09/14 19:31:06  darkeye
  added IceCast2 / vorbis support



------------------------------------------------------------------------------*/

