/*------------------------------------------------------------------------------

   Copyright (c) 2000 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : ExternalEncoder.h
   Version  : $Revision$
   Author   : $Author$
   Location : $ExternalEncoder$
   
   Abstract : 

     A class representing an external audio encoder which is invoked
     with a frok() and an exec() call

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
#ifndef EXTERNAL_ENCODER_H
#define EXTERNAL_ENCODER_H

#ifndef __cplusplus
#error This is a C++ include file
#endif


/* ============================================================ include files */

#ifdef HAVE_CONFIG_H
#include "configure.h"
#endif

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#else
#error need sys/types.h
#endif


#include "Exception.h"
#include "AudioEncoder.h"


/* ================================================================ constants */


/* =================================================================== macros */


/* =============================================================== data types */

/*------------------------------------------------------------------------------
 *  
 *----------------------------------------------------------------------------*/
class ExternalEncoder : public AudioEncoder
{
    private:

        char      * encoderName;
        char      * inFileName;
        char      * outFileName;

        static const unsigned int       numCmdArgs = 32;

        char      * cmdArgs[numCmdArgs];

        pid_t       child;


        void
        init (  const char    * encoderName,
                const char    * inFileName,
                const char    * outFileName )           throw ( Exception );


        void
        strip ( void )                                  throw ( Exception );


    protected:

        inline
        ExternalEncoder ( void )                        throw ( Exception )
        {
            throw Exception( __FILE__, __LINE__);
        }


        void
        setArg (    const char    * str,
                    unsigned int    index )     throw ( Exception );


        virtual unsigned int
        makeArgs ( void )               throw ( Exception )     = 0;


    public:

        inline
        ExternalEncoder (   const char    * encoderName,
                            const char    * inFileName,
                            unsigned int    inSampleRate,
                            unsigned int    inBitsPerSample,
                            unsigned int    inChannel,
                            const char    * outFileName,
                            unsigned int    outBitrate,
                            unsigned int    outSampleRate = 0,
                            unsigned int    outChannel    = 0 )
                                                        throw ( Exception )
            
                    : AudioEncoder ( inSampleRate,
                                     inBitsPerSample,
                                     inChannel, 
                                     outBitrate,
                                     outSampleRate,
                                     outChannel )
        {
            init ( encoderName, inFileName, outFileName );
        }


        inline
        ExternalEncoder (   const char            * encoderName,
                            const char            * inFileName,
                            const AudioSource     * as,
                            const char            * outFileName,
                            unsigned int            outBitrate,
                            unsigned int            outSampleRate = 0,
                            unsigned int            outChannel    = 0 )
                                                            throw ( Exception )
            
                    : AudioEncoder ( as,
                                     outBitrate,
                                     outSampleRate,
                                     outChannel )
        {
            init ( encoderName, inFileName, outFileName );
        }


        inline
        ExternalEncoder (  const ExternalEncoder &    encoder )
                                                            throw ( Exception )
                    : AudioEncoder( encoder )
        {
            init( encoder.encoderName,
                  encoder.inFileName,
                  encoder.outFileName );
        }


        inline virtual
        ~ExternalEncoder ( void )                           throw ( Exception )
        {
            strip();
        }


        inline virtual ExternalEncoder &
        operator= ( const ExternalEncoder &     encoder )   throw ( Exception )
        {
            if ( this != &encoder ) {
                strip();
                AudioEncoder::operator=( encoder);
                init( encoder.encoderName,
                      encoder.inFileName,
                      encoder.outFileName );
            }

            return *this;
        }


        const char *
        getEncoderName ( void ) const           throw ()
        {
            return encoderName;
        }


        const char *
        getInFileName ( void ) const            throw ()
        {
            return inFileName;
        }


        const char *
        getOutFileName ( void ) const           throw ()
        {
            return outFileName;
        }


        inline virtual bool
        isRunning ( void ) const                throw ()
        {
            return child != 0;
        }


        virtual bool
        start ( void )                          throw ( Exception );


        virtual void
        stop ( void )                           throw ( Exception );
};


/* ================================================= external data structures */


/* ====================================================== function prototypes */



#endif  /* EXTERNAL_ENCODER_H */


/*------------------------------------------------------------------------------
 
  $Source$

  $Log$
  Revision 1.3  2000/11/05 17:37:24  darkeye
  removed clone() functions

  Revision 1.2  2000/11/05 14:08:27  darkeye
  changed builting to an automake / autoconf environment

  Revision 1.1.1.1  2000/11/05 10:05:50  darkeye
  initial version

  
------------------------------------------------------------------------------*/

