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

/*------------------------------------------------------------------------------
 *  
 *----------------------------------------------------------------------------*/
class LameEncoder : public ExternalEncoder
{
    private:

        unsigned int    lowpass;
        unsigned int    highpass;

    protected:

        virtual unsigned int
        makeArgs ( void )                       throw ( Exception );


        inline
        LameEncoder ( void )                    throw ( Exception )
        {
            throw Exception( __FILE__, __LINE__);
        }


        inline void
        init (  unsigned int    lowpass,
                unsigned int    highpass )      throw ( Exception )
        {
            this->lowpass  = lowpass;
            this->highpass = highpass;
        }


        inline void
        strip ( void )                          throw ( Exception )
        {
        }


    public:

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


        inline
        LameEncoder (   const LameEncoder & encoder )       throw ( Exception )
                    : ExternalEncoder( encoder )
        {
            init( encoder.lowpass, encoder.highpass);
        }


        inline virtual
        ~LameEncoder ( void )                               throw ( Exception )
        {
            strip();
        }


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


        inline virtual LameEncoder *
        clone ( void ) const                                throw ( Exception )
        {
            return new LameEncoder( *this );
        }


        inline unsigned int
        getLowpass ( void ) const                       throw ()
        {
            return lowpass;
        }


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


        inline unsigned int
        getHighpass ( void ) const                      throw ()
        {
            return highpass;
        }


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
  Revision 1.1  2000/11/05 10:05:52  darkeye
  Initial revision

  
------------------------------------------------------------------------------*/

