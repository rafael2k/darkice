/*------------------------------------------------------------------------------

   Copyright (c) 2000 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : AudioSource.h
   Version  : $Revision$
   Author   : $Author$
   Location : $Source$
   
   Abstract : 

     Audio data input

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
#ifndef AUDIO_SOURCE_H
#define AUDIO_SOURCE_H

#ifndef __cplusplus
#error This is a C++ include file
#endif


/* ============================================================ include files */

#include "Source.h"


/* ================================================================ constants */


/* =================================================================== macros */


/* =============================================================== data types */

/*------------------------------------------------------------------------------
 *  
 *----------------------------------------------------------------------------*/
class AudioSource : public Source
{
    private:

        unsigned int    channel;
        unsigned int    sampleRate;
        unsigned int    bitsPerSample;


        inline void
        init (   unsigned int   sampleRate,
                 unsigned int   bitsPerSample,
                 unsigned int   channel )               throw ( Exception )
        {
            this->sampleRate     = sampleRate;
            this->bitsPerSample  = bitsPerSample;
            this->channel        = channel;
        }


        inline void
        strip ( void )                                  throw ( Exception )
        {
        }


    protected:

        inline
        AudioSource (   unsigned int    sampleRate    = 44100,
                        unsigned int    bitsPerSample = 16,
                        unsigned int    channel       = 2 )
                                                        throw ( Exception )
        {
            init ( sampleRate, bitsPerSample, channel);
        }


        inline
        AudioSource (   const AudioSource &     as )    throw ( Exception )
            : Source( as )
        {
            init ( as.sampleRate, as.bitsPerSample, as.channel);
        }


        virtual inline
        ~AudioSource ( void )                           throw ( Exception )
        {
        }


        inline virtual AudioSource &
        operator= (     const AudioSource &     as )    throw ( Exception )
        {
            if ( this != &as ) {
                strip();
                Source::operator=( as );
                init ( as.sampleRate, as.bitsPerSample, as.channel);
            }

            return *this;
        }


    public:

        inline unsigned int
        getChannel ( void ) const           throw ()
        {
            return channel;
        }


        inline unsigned int
        getSampleRate ( void ) const        throw ()
        {
            return sampleRate;
        }


        inline unsigned int
        getBitsPerSample ( void ) const     throw ()
        {
            return bitsPerSample;
        }
};


/* ================================================= external data structures */


/* ====================================================== function prototypes */



#endif  /* AUDIO_SOURCE_H */


/*------------------------------------------------------------------------------
 
  $Source$

  $Log$
  Revision 1.2  2000/11/05 17:37:24  darkeye
  removed clone() functions

  Revision 1.1.1.1  2000/11/05 10:05:47  darkeye
  initial version

  
------------------------------------------------------------------------------*/

