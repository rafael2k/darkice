/*------------------------------------------------------------------------------

   Copyright (c) 2000-2007 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : LameLibEncoder.h
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
#ifndef LAME_LIB_ENCODER_H
#define LAME_LIB_ENCODER_H

#ifndef __cplusplus
#error This is a C++ include file
#endif


/* ============================================================ include files */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_LAME_LIB
#include <lame/lame.h>
#else
#error configure with lame
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
 *  A class representing the lame encoder linked as a shared object or as
 *  a static library.
 *
 *  @author  $Author: rafael@riseup.net $
 *  @version $Revision: 474 $
 */
class LameLibEncoder : public AudioEncoder, public virtual Reporter
{
    private:

        /**
         *  Lame library global flags
         */
        lame_global_flags             * lameGlobalFlags;

        /**
         *  Lowpass filter. Sound frequency in Hz, from where up the
         *  input is cut.
         */
        int                             lowpass;

        /**
         *  Highpass filter. Sound frequency in Hz, from where down the
         *  input is cut.
         */
        int                             highpass;

        /**
         *  Initialize the object.
         *
         *  @param lowpass frequency threshold for the lowpass filter.
         *                 Input above this frequency is cut.
         *                 If 0, lame's default values are used,
         *                 which depends on the out sample rate.
         *  @param highpass frequency threshold for the highpass filter.
         *                  Input below this frequency is cut.
         *                  If 0, lame's default values are used,
         *                  which depends on the out sample rate.
         *  @exception Exception
         */
        inline void
        init ( int              lowpass,
               int              highpass )              throw ( Exception )
        {
            this->lameGlobalFlags = NULL;
            this->lowpass         = lowpass;
            this->highpass        = highpass;

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
            if ( getInChannel() < getOutChannel() ) {
                throw Exception( __FILE__, __LINE__,
                                 "output channels greater then input channels",
                                 getOutChannel() );
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
        LameLibEncoder ( void )                         throw ( Exception )
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
         *                 If 0, lame's default values are used,
         *                 which depends on the out sample rate.
         *  @param highpass frequency threshold for the highpass filter.
         *                  Input below this frequency is cut.
         *                  If 0, lame's default values are used,
         *                  which depends on the out sample rate.
         *  @exception Exception
         */
        inline
        LameLibEncoder (    Sink          * sink,
                            unsigned int    inSampleRate,
                            unsigned int    inBitsPerSample,
                            unsigned int    inChannel,
                            bool            inBigEndian,
                            BitrateMode     outBitrateMode,
                            unsigned int    outBitrate,
                            double          outQuality,
                            unsigned int    outSampleRate = 0,
                            unsigned int    outChannel    = 0,
                            int             lowpass       = 0,
                            int             highpass      = 0 )
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
            init( lowpass, highpass);
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
         *                 If 0, lame's default values are used,
         *                 which depends on the out sample rate.
         *  @param highpass frequency threshold for the highpass filter.
         *                  Input below this frequency is cut.
         *                  If 0, lame's default values are used,
         *                  which depends on the out sample rate.
         *  @exception Exception
         */
        inline
        LameLibEncoder (    Sink                  * sink,
                            const AudioSource     * as,
                            BitrateMode             outBitrateMode,
                            unsigned int            outBitrate,
                            double                  outQuality,
                            unsigned int            outSampleRate = 0,
                            unsigned int            outChannel    = 0,
                            int                     lowpass       = 0,
                            int                     highpass      = 0 )
                                                            throw ( Exception )
            
                    : AudioEncoder ( sink,
                                     as,
                                     outBitrateMode,
                                     outBitrate,
                                     outQuality,
                                     outSampleRate,
                                     outChannel )
        {
            init( lowpass, highpass);
        }

        /**
         *  Copy constructor.
         *
         *  @param encoder the LameLibEncoder to copy.
         */
        inline
        LameLibEncoder (  const LameLibEncoder &    encoder )
                                                            throw ( Exception )
                    : AudioEncoder( encoder )
        {
            init( encoder.lowpass, encoder.highpass );
        }
         

        /**
         *  Destructor.
         *
         *  @exception Exception
         */
        inline virtual
        ~LameLibEncoder ( void )                            throw ( Exception )
        {
            if ( isOpen() ) {
                close();
            }
            strip();
        }

        /**
         *  Assignment operator.
         *
         *  @param encoder the LameLibEncoder to assign this to.
         *  @return a reference to this LameLibEncoder.
         *  @exception Exception
         */
        inline virtual LameLibEncoder &
        operator= ( const LameLibEncoder &      encoder )   throw ( Exception )
        {
            if ( this != &encoder ) {
                strip();
                AudioEncoder::operator=( encoder);
                init( encoder.lowpass, encoder.highpass );
            }

            return *this;
        }

        /**
         *  Get the version string of the underlying lame library.
         *
         *  @return the version string of the underlying lame library.
         */
        inline const char *
        getLameVersion( void )
        {
            return get_lame_version();
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
            return lameGlobalFlags != 0;
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


#endif  /* LAME_LIB_ENCODER_H */

