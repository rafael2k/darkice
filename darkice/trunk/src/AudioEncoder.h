/*------------------------------------------------------------------------------

   Copyright (c) 2000 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : AudioEncoder.h
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
#ifndef AUDIO_ENCODER_H
#define AUDIO_ENCODER_H

#ifndef __cplusplus
#error This is a C++ include file
#endif


/* ============================================================ include files */

#include "Referable.h"
#include "Sink.h"
#include "AudioSource.h"


/* ================================================================ constants */


/* =================================================================== macros */


/* =============================================================== data types */

/**
 *  An audio encoder
 *
 *  @author  $Author$
 *  @version $Revision$
 */
class AudioEncoder : public Sink, public virtual Referable
{
    public:
        /**
         *  Type to specify bitrate mode. Possible values:
         *  - cbr - constant bitrate mode
         *          described by bitrate
         *  - abr - average bitrate mode
         *          described by an average bitrate and quality
         *  - vbr - variable bitrate mode
         *          described by quality
         */
        enum BitrateMode { cbr, abr, vbr };

    private:

        /**
         *  Sample rate of the input.
         */
        unsigned int        inSampleRate;

        /**
         *  Number of bits per sample of the input.
         */
        unsigned int        inBitsPerSample;

        /**
         *  Number of channels of the input.
         */
        unsigned int        inChannel;

        /**
         *  Is the input big endian or little endian?
         */
        bool                inBigEndian;

        /**
         *  The bitrate mode of the encoder
         */
        BitrateMode         outBitrateMode;

        /**
         *  Bit rate of the output in kbits/sec, for fixed bitrate encodings.
         */
        unsigned int        outBitrate;

        /**
         *  Quality of the output, for variable bitrate encodings.
         */
        double              outQuality;

        /**
         *  Sample rate of the output.
         */
        unsigned int        outSampleRate;

        /**
         *  Number of channels of the output.
         */
        unsigned int        outChannel;

        /**
         *  Initialize the object.
         *
         *  @param inSampleRate sample rate of the input.
         *  @param inBitsPerSample number of bits per sample of the input.
         *  @param inChannel number of channels  of the input.
         *  @param inBigEndian shows if the input is big or little endian.
         *  @param outBitrateMode the bit rate mode of the output.
         *  @param outBitrate bit rate of the output.
         *  @param outSampleRate sample rate of the output.
         *  @param outChannel number of channels of the output.
         *  @exception Exception
         */
        inline void
        init (      unsigned int    inSampleRate,
                    unsigned int    inBitsPerSample,
                    unsigned int    inChannel,
                    bool            inBigEndian,
                    BitrateMode     outBitrateMode,
                    unsigned int    outBitrate,
                    double          outQuality,
                    unsigned int    outSampleRate,
                    unsigned int    outChannel )        throw ( Exception )
        {
            this->inSampleRate     = inSampleRate;
            this->inBitsPerSample  = inBitsPerSample;
            this->inChannel        = inChannel;
            this->inBigEndian      = inBigEndian;
            this->outBitrateMode   = outBitrateMode;
            this->outBitrate       = outBitrate;
            this->outQuality       = outQuality;
            this->outSampleRate    = outSampleRate;
            this->outChannel       = outChannel;

            if ( outQuality < 0 || 1.0 < outQuality ) {
                throw Exception( __FILE__, __LINE__, "invalid encoder quality");
            }
        }

        /**
         *  De-iitialize the object.
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
        AudioEncoder ( void )                           throw ( Exception )
        {
            throw Exception( __FILE__, __LINE__);
        }

        /**
         *  Constructor.
         *
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
         *  @exception Exception
         */
        inline
        AudioEncoder (  unsigned int    inSampleRate,
                        unsigned int    inBitsPerSample,
                        unsigned int    inChannel, 
                        bool            inBigEndian,
                        BitrateMode     outBitrateMode,
                        unsigned int    outBitrate,
                        double          outQuality,
                        unsigned int    outSampleRate = 0,
                        unsigned int    outChannel    = 0 )
                                                        throw ( Exception )
        {
            init ( inSampleRate,
                   inBitsPerSample,
                   inChannel,
                   inBigEndian,
                   outBitrateMode,
                   outBitrate,
                   outQuality,
                   outSampleRate ? outSampleRate : inSampleRate,
                   outChannel    ? outChannel    : inChannel );
        }

        /**
         *  Constructor.
         *
         *  @param as get input sample rate, bits per sample and channels
         *            from this AudioSource.
         *  @param outBitrateMode the bit rate mode of the output.
         *  @param outBitrate bit rate of the output (kbits/sec).
         *  @param outQuality the quality of the stream.
         *  @param outSampleRate sample rate of the output.
         *                       If 0, input sample rate is used.
         *  @param outChannel number of channels of the output.
         *                    If 0, input channel is used.
         *  @exception Exception
         */
        inline
        AudioEncoder (  const AudioSource     * as,
                        BitrateMode             outBitrateMode,
                        unsigned int            outBitrate,
                        double                  outQuality,
                        unsigned int            outSampleRate = 0,
                        unsigned int            outChannel    = 0 )
                                                        throw ( Exception)
        {
            init( as->getSampleRate(),
                  as->getBitsPerSample(),
                  as->getChannel(),
                  as->isBigEndian(),
                  outBitrateMode,
                  outBitrate,
                  outQuality,
                  outSampleRate ? outSampleRate : as->getSampleRate(),
                  outChannel    ? outChannel    : as->getChannel() );
        }

        /**
         *  Copy constructor.
         *
         *  @param encoder the AudioEncoder to copy.
         */
        inline
        AudioEncoder (  const AudioEncoder &    encoder )   throw ( Exception )
        {
            init ( encoder.inSampleRate,
                   encoder.inBitsPerSample,
                   encoder.inChannel,
                   encoder.inBigEndian,
                   encoder.outBitrateMode,
                   encoder.outBitrate,
                   encoder.outQuality,
                   encoder.outSampleRate,
                   encoder.outChannel );
        }

        /**
         *  Assignment operator.
         *
         *  @param encoder the AudioEncoder to assign this to.
         *  @return a reference to this AudioEncoder.
         *  @exception Exception
         */
        inline virtual AudioEncoder &
        operator= ( const AudioEncoder &        encoder )   throw ( Exception )
        {
            if ( this != &encoder ) {
                strip();

                init ( encoder.inSampleRate,
                       encoder.inBitsPerSample,
                       encoder.inChannel,
                       encoder.inBigEndian,
                       encoder.outBitrateMode,
                       encoder.outBitrate,
                       encoder.outQuality,
                       encoder.outSampleRate,
                       encoder.outChannel );
            }

            return *this;
        }


    public:

        /**
         *  Destructor.
         *
         *  @exception Exception
         */
        inline virtual
        ~AudioEncoder ( void )          throw ( Exception )
        {
            strip();
        }

        /**
         *  Get the number of channels of the input.
         *
         *  @return the number of channels  of the input.
         */
        inline int
        getInChannel ( void ) const         throw ()
        {
            return inChannel;
        }

        /**
         *  Tell if the input is big or little endian.
         *
         *  @return true if the input is big endian, false if little endian.
         */
        inline bool
        isInBigEndian ( void ) const         throw ()
        {
            return inBigEndian;
        }

        /**
         *  Get the sample rate of the input.
         *
         *  @return the sample rate of the input.
         */
        inline int
        getInSampleRate ( void ) const      throw ()
        {
            return inSampleRate;
        }

        /**
         *  Get the number of bits per sample of the input.
         *
         *  @return the number of bits per sample of the input.
         */
        inline int
        getInBitsPerSample ( void ) const   throw ()
        {
            return inBitsPerSample;
        }

        /**
         *  Get the number of channels of the output.
         *
         *  @return the number of channels of the output.
         */
        inline int
        getOutChannel ( void ) const        throw ()
        {
            return outChannel;
        }

        /**
         *  Get the sample rate of the output.
         *
         *  @return the sample rate of the output.
         */
        inline int
        getOutSampleRate ( void ) const     throw ()
        {
            return outSampleRate;
        }

        /**
         *  Get the bit rate mode of the output.
         *
         *  @return the bit rate mode of the output.
         */
        inline BitrateMode
        getOutBitrateMode ( void ) const        throw ()
        {
            return outBitrateMode;
        }

        /**
         *  Get the bit rate of the output in kbits/sec, for fixed bitrate
         *  encodings.
         *
         *  @return the bit rate of the output.
         */
        inline unsigned int
        getOutBitrate ( void ) const        throw ()
        {
            return outBitrate;
        }

        /**
         *  Get the encoding quality of the output, for variable bitrate
         *  encodings.
         *
         *  @return the encoding quality of the output.
         */
        inline double
        getOutQuality ( void ) const        throw ()
        {
            return outQuality;
        }

        /**
         *  Check wether encoding is in progress.
         *
         *  @return true if encoding is in progress, false otherwise.
         */
        virtual bool
        isRunning ( void ) const           throw ()                 = 0;

        /**
         *  Start encoding. This function returns as soon as possible,
         *  with encoding started in the background.
         *
         *  @return true if encoding has started, false otherwise.
         *  @exception Exception
         */
        virtual bool
        start ( void )                      throw ( Exception )     = 0;

        /**
         *  Stop encoding. Stops the encoding running in the background.
         *
         *  @exception Exception
         */
        virtual void
        stop ( void )                       throw ( Exception )     = 0;
};


/* ================================================= external data structures */


/* ====================================================== function prototypes */



#endif  /* AUDIO_ENCODER_H */


/*------------------------------------------------------------------------------
 
  $Source$

  $Log$
  Revision 1.8  2002/08/20 19:35:37  darkeye
  added possibility to specify maximum bitrate for Ogg Vorbis streams

  Revision 1.7  2002/04/13 11:26:00  darkeye
  added cbr, abr and vbr setting feature with encoding quality

  Revision 1.6  2002/03/28 16:39:32  darkeye
  added interface for variable bitrate encoding

  Revision 1.5  2002/02/19 15:23:59  darkeye
  fixed typo

  Revision 1.4  2001/09/18 14:57:19  darkeye
  finalized Solaris port

  Revision 1.3  2001/09/14 19:31:06  darkeye
  added IceCast2 / vorbis support

  Revision 1.2  2000/11/12 14:54:50  darkeye
  added kdoc-style documentation comments

  Revision 1.1.1.1  2000/11/05 10:05:47  darkeye
  initial version

  
------------------------------------------------------------------------------*/

