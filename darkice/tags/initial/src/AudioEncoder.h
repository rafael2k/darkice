/*------------------------------------------------------------------------------

   Copyright (c) 2000 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : AudioEncoder.h
   Version  : $Revision$
   Author   : $Author$
   Location : $AudioEncoder$
   
   Abstract : 

     An audio encoder

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
#ifndef AUDIO_ENCODER_H
#define AUDIO_ENCODER_H

#ifndef __cplusplus
#error This is a C++ include file
#endif


/* ============================================================ include files */

#include "Referable.h"
#include "AudioSource.h"


/* ================================================================ constants */


/* =================================================================== macros */


/* =============================================================== data types */

/*------------------------------------------------------------------------------
 *  
 *----------------------------------------------------------------------------*/
class AudioEncoder : public virtual Referable
{
    private:

        unsigned int        inSampleRate;
        unsigned int        inBitsPerSample;
        unsigned int        inChannel;

        unsigned int        outBitrate;
        unsigned int        outSampleRate;
        unsigned int        outChannel;


        inline void
        init (      unsigned int    inSampleRate,
                    unsigned int    inBitsPerSample,
                    unsigned int    inChannel,
                    unsigned int    outBitrate,
                    unsigned int    outSampleRate,
                    unsigned int    outChannel )        throw ( Exception )
        {
            this->inSampleRate     = inSampleRate;
            this->inBitsPerSample  = inBitsPerSample;
            this->inChannel        = inChannel;
            this->outBitrate       = outBitrate;
            this->outSampleRate    = outSampleRate;
            this->outChannel       = outChannel;
        }


        inline void
        strip ( void )                                  throw ( Exception )
        {
        }


    protected:

        inline
        AudioEncoder ( void )                           throw ( Exception )
        {
            throw Exception( __FILE__, __LINE__);
        }


        inline
        AudioEncoder (  unsigned int    inSampleRate,
                        unsigned int    inBitsPerSample,
                        unsigned int    inChannel, 
                        unsigned int    outBitrate,
                        unsigned int    outSampleRate = 0,
                        unsigned int    outChannel    = 0 )
                                                        throw ( Exception )
        {
            init ( inSampleRate,
                   inBitsPerSample,
                   inChannel, 
                   outBitrate,
                   outSampleRate ? outSampleRate : inSampleRate,
                   outChannel    ? outChannel    : inChannel );
        }


        inline
        AudioEncoder (  const AudioSource     * as,
                        unsigned int            outBitrate,
                        unsigned int            outSampleRate = 0,
                        unsigned int            outChannel    = 0 )
                                                        throw ( Exception)
        {
            init( as->getSampleRate(),
                  as->getBitsPerSample(),
                  as->getChannel(), 
                  outBitrate,
                  outSampleRate ? outSampleRate : as->getSampleRate(),
                  outChannel    ? outChannel    : as->getChannel() );
        }


        inline
        AudioEncoder (  const AudioEncoder &    encoder )   throw ( Exception )
        {
            init ( encoder.inSampleRate,
                   encoder.inBitsPerSample,
                   encoder.inChannel,
                   encoder.outBitrate,
                   encoder.outSampleRate,
                   encoder.outChannel );
        }


        inline virtual AudioEncoder &
        operator= ( const AudioEncoder &        encoder )   throw ( Exception )
        {
            if ( this != &encoder ) {
                strip();

                init ( encoder.inSampleRate,
                       encoder.inBitsPerSample,
                       encoder.inChannel,
                       encoder.outBitrate,
                       encoder.outSampleRate,
                       encoder.outChannel );
            }

            return *this;
        }


    public:

        inline virtual
        ~AudioEncoder ( void )          throw ( Exception )
        {
            strip();
        }


        inline int
        getInChannel ( void ) const         throw ()
        {
            return inChannel;
        }


        inline int
        getInSampleRate ( void ) const      throw ()
        {
            return inSampleRate;
        }


        inline int
        getInBitsPerSample ( void ) const   throw ()
        {
            return inBitsPerSample;
        }


        inline int
        getOutChannel ( void ) const        throw ()
        {
            return outChannel;
        }


        inline int
        getOutSampleRate ( void ) const     throw ()
        {
            return outSampleRate;
        }


        inline int
        getOutBitrate ( void ) const        throw ()
        {
            return outBitrate;
        }


        virtual bool
        isRunning ( void ) const           throw ()                 = 0;


        virtual bool
        start ( void )                      throw ( Exception )     = 0;


        virtual void
        stop ( void )                       throw ( Exception )     = 0;
};


/* ================================================= external data structures */


/* ====================================================== function prototypes */



#endif  /* AUDIO_ENCODER_H */


/*------------------------------------------------------------------------------
 
  $Source$

  $Log$
  Revision 1.1  2000/11/05 10:05:47  darkeye
  Initial revision

  
------------------------------------------------------------------------------*/

