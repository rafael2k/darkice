/*------------------------------------------------------------------------------

   Copyright (c) 2000 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : LameEncoder.h
   Version  : $Revision$
   Author   : $Author$
   Location : $LameEncoder$
   
   Abstract : 

     A class representing the lame mp3 encoder

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
     Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307,
     USA.

------------------------------------------------------------------------------*/
#ifndef LAME_ENCODER_H
#define LAME_ENCODER_H

#ifndef __cplusplus
#error This is a C++ include file
#endif


/* ============================================================ include files */

#include "Exception.h"
#include "ExternalEncoder.h"


/* ================================================================ constants */


/* =================================================================== macros */


/* =============================================================== data types */

/**
 *  A class representing the lame mp3 encoder
 *
 *  @author  $Author$
 *  @version $Revision$
 */
class LameEncoder : public ExternalEncoder
{
    private:

        /**
         *  Highpass filter. Sound frequency in Hz, from where up the
         *  input is cut.
         */
        unsigned int    lowpass;

        /**
         *  Lowpass filter. Sound frequency in Hz, from where down the
         *  input is cut.
         */
        unsigned int    highpass;

    protected:

        /**
         *  Fill in the list of command line arguments. Puts a 0
         *  as the last in the list of args.
         *
         *  @return the number of arguments filled.
         *  @exception Exception
         */
        virtual unsigned int
        makeArgs ( void )                       throw ( Exception );

        /**
         *  Default constructor. Always throws an Exception.
         *
         *  @exception Exception
         */
        inline
        LameEncoder ( void )                    throw ( Exception )
        {
            throw Exception( __FILE__, __LINE__);
        }

        /**
         *  Initialize the object.
         *
         *  @param lowpass lowpass filter range.
         *  @param highpass highpass filter range.
         *  @exception Exception
         */
        inline void
        init (  unsigned int    lowpass,
                unsigned int    highpass )      throw ( Exception )
        {
            this->lowpass  = lowpass;
            this->highpass = highpass;
        }

        /**
         *  De-initialize the object.
         *
         *  @exception Exception
         */
        inline void
        strip ( void )                          throw ( Exception )
        {
        }


    public:

        /**
         *  Constructor.
         *
         *  @param encoderName name of the encoder.
         *                     (the command to invoke the encoder with)
         *  @param inFileName input file parameter for the encoder.
         *  @param inSampleRate sample rate of the input.
         *  @param inBitsPerSample number of bits per sample of the input.
         *  @param inChannel number of channels  of the input.
         *  @param outFileName output file parameter for the encoder.
         *  @param outBitrate bit rate of the output (bits/sec).
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
        LameEncoder (   const char    * encoderName,
                        const char    * inFileName,
                        unsigned int    inSampleRate,
                        unsigned int    inBitsPerSample,
                        unsigned int    inChannel,
                        const char    * outFileName,
                        unsigned int    outBitrate,
                        unsigned int    outSampleRate = 0,
                        unsigned int    outChannel    = 0,
                        unsigned int    lowpass       = 0,
                        unsigned int    highpass      = 0 )
                                                            throw ( Exception )
            
                    : ExternalEncoder ( encoderName,
                                        inFileName,
                                        inSampleRate,
                                        inBitsPerSample,
                                        inChannel,
                                        outFileName,
                                        outBitrate,
                                        outSampleRate,
                                        outChannel )
        {
            init( lowpass, highpass);
        }

        /**
         *  Constructor.
         *
         *  @param encoderName name of the encoder.
         *                     (the command to invoke the encoder with)
         *  @param inFileName input file parameter for the encoder.
         *  @param as get input sample rate, bits per sample and channels
         *            from this AudioSource.
         *  @param outFileName output file parameter for the encoder.
         *  @param outBitrate bit rate of the output (bits/sec).
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
        LameEncoder (   const char            * encoderName,
                        const char            * inFileName,
                        const AudioSource     * as,
                        const char            * outFileName,
                        unsigned int            outBitrate,
                        unsigned int            outSampleRate = 0,
                        unsigned int            outChannel    = 0,
                        unsigned int            lowpass       = 0,
                        unsigned int            highpass      = 0 )
                                                            throw ( Exception )
            
                    : ExternalEncoder ( encoderName,
                                        inFileName,
                                        as,
                                        outFileName,
                                        outBitrate,
                                        outSampleRate,
                                        outChannel )
        {
            init( lowpass, highpass);
        }

        /**
         *  Copy constructor.
         *
         *  @param encoder the LameEncoder to copy.
         */
        inline
        LameEncoder (   const LameEncoder & encoder )       throw ( Exception )
                    : ExternalEncoder( encoder )
        {
            init( encoder.lowpass, encoder.highpass);
        }

        /**
         *  Destructor.
         *
         *  @exception Exception
         */
        inline virtual
        ~LameEncoder ( void )                               throw ( Exception )
        {
            strip();
        }

        /**
         *  Assignment operator.
         *
         *  @param encoder the LameEncoder to assign this to.
         *  @return a reference to this LameEncoder.
         *  @exception Exception
         */
        inline virtual LameEncoder &
        operator= ( const LameEncoder & encoder )           throw ( Exception )
        {
            if ( this != &encoder ) {
                strip();
                ExternalEncoder::operator=( encoder);
                init( encoder.lowpass, encoder.highpass);
            }

            return *this;
        }

        /**
         *  Get the lowpass filter threshold. Sound frequency in Hz,
         *  from where up the input is cut.
         *
         *  @return the lowpass filter threshold.
         */
        inline unsigned int
        getLowpass ( void ) const                       throw ()
        {
            return lowpass;
        }

        /**
         *  Set the lowpass filter threshold. Sound frequency in Hz,
         *  from where up the input is cut.
         *  Can be only set if encoding is not in progress.
         *
         *  @param lowpass the lowpass filter threshold.
         *  @return if setting is successful.
         */
        inline bool
        setLowpass (    unsigned int    lowpass )       throw ()
        {
            if ( isRunning() ) {
                return false;
            } else {
                this->lowpass = lowpass;
                return true;
            }
        }

        /**
         *  Get the highpass filter threshold. Sound frequency in Hz,
         *  from where down the input is cut.
         *
         *  @return the highpass filter threshold.
         */
        inline unsigned int
        getHighpass ( void ) const                      throw ()
        {
            return highpass;
        }

        /**
         *  Set the highpass filter threshold. Sound frequency in Hz,
         *  from where down the input is cut.
         *  Can be only set if encoding is not in progress.
         *
         *  @param highpass the highpass filter threshold.
         *  @return if setting is successful.
         */
        inline bool
        setHighpass (   unsigned int    highpass )      throw ()
        {
            if ( isRunning() ) {
                return false;
            } else {
                this->highpass = highpass;
                return true;
            }
        }


};


/* ================================================= external data structures */


/* ====================================================== function prototypes */



#endif  /* LAME_ENCODER_H */


/*------------------------------------------------------------------------------
 
  $Source$

  $Log$
  Revision 1.3  2000/11/12 14:54:50  darkeye
  added kdoc-style documentation comments

  Revision 1.2  2000/11/05 17:37:24  darkeye
  removed clone() functions

  Revision 1.1.1.1  2000/11/05 10:05:52  darkeye
  initial version

  
------------------------------------------------------------------------------*/

