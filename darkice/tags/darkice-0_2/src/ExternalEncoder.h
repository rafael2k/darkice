/*------------------------------------------------------------------------------

   Copyright (c) 2000 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : ExternalEncoder.h
   Version  : $Revision$
   Author   : $Author$
   Location : $ExternalEncoder$
   
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
#include "Reporter.h"
#include "AudioEncoder.h"


/* ================================================================ constants */


/* =================================================================== macros */


/* =============================================================== data types */

/**
 *  A class representing an external audio encoder which is invoked
 *  with a frok() and an exec() call
 *
 *  @author  $Author$
 *  @version $Revision$
 */
class ExternalEncoder : public AudioEncoder, public virtual Reporter
{
    private:

        /**
         *  Name of the encoder (the command to invoke the encoder with).
         */
        char      * encoderName;

        /**
         *  Input file parameter for the encoder.
         */
        char      * inFileName;

        /**
         *  Output file parameter for the encoder.
         */
        char      * outFileName;

        /**
         *  Maximum number of command line arguments.
         */
        static const unsigned int       numCmdArgs = 64;

        /**
         *  Array of command line arguments.
         */
        char      * cmdArgs[numCmdArgs];

        /**
         *  Process ID of the encoder process.
         */
        pid_t       child;

        /**
         *  Initialize the object.
         *
         *  @param encoderName name of the encoder.
         *  @param inFileName input file parameter for the encoder.
         *  @param outFileName output file parameter for the encoder.
         *  @exception Exception
         */
        void
        init (  const char    * encoderName,
                const char    * inFileName,
                const char    * outFileName )           throw ( Exception );

        /**
         *  De-initialize the object.
         *
         *  @exception Exception
         */
        void
        strip ( void )                                  throw ( Exception );


    protected:

        /**
         *  Default constructor. Always throws an Exception.
         *
         *  @exception Exception
         */
        inline
        ExternalEncoder ( void )                        throw ( Exception )
        {
            throw Exception( __FILE__, __LINE__);
        }

        /**
         *  Set a command line argument in the argument array.
         *
         *  @param str the argument to set.
         *  @param index the place in the array to set the argument.
         *  @exception Exception
         */
        void
        setArg (    const char    * str,
                    unsigned int    index )     throw ( Exception );

        /**
         *  Fill in the list of command line arguments. Puts a 0
         *  as the last in the list of args.
         *
         *  @return the number of arguments filled.
         *  @exception Exception
         */
        virtual unsigned int
        makeArgs ( void )               throw ( Exception )     = 0;


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
         *  @exception Exception
         */
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
         *                       If 0, input sample rate is used.
         *  @param outChannel number of channels of the output.
         *                    If 0, input channel is used.
         *  @exception Exception
         */
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

        /**
         *  Copy constructor.
         *
         *  @param encoder the ExternalEncoder to copy.
         */
        inline
        ExternalEncoder (  const ExternalEncoder &    encoder )
                                                            throw ( Exception )
                    : AudioEncoder( encoder )
        {
            init( encoder.encoderName,
                  encoder.inFileName,
                  encoder.outFileName );
        }

        /**
         *  Destructor.
         *
         *  @exception Exception
         */
        inline virtual
        ~ExternalEncoder ( void )                           throw ( Exception )
        {
            strip();
        }

        /**
         *  Assignment operator.
         *
         *  @param encoder the ExternalEncoder to assign this to.
         *  @return a reference to this ExternalEncoder.
         *  @exception Exception
         */
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

        /**
         *  Get the name of the encoder
         *  (the command to invoke the encoder with).
         *
         *  @return the name of the encoder.
         */
        const char *
        getEncoderName ( void ) const           throw ()
        {
            return encoderName;
        }

        /**
         *  Get the input file parameter for the encoder.
         *
         *  @return the input file parameter for the encoder.
         */
        const char *
        getInFileName ( void ) const            throw ()
        {
            return inFileName;
        }

        /**
         *  Get the output file parameter for the encoder.
         *
         *  @return the output file parameter for the encoder.
         */
        const char *
        getOutFileName ( void ) const           throw ()
        {
            return outFileName;
        }

        /**
         *  Check wether encoding is in progress.
         *
         *  @return true if encoding is in progress, false otherwise.
         */
        inline virtual bool
        isRunning ( void ) const                throw ()
        {
            return child != 0;
        }

        /**
         *  Start encoding. This function returns as soon as possible,
         *  with encoding started as a separate process in the
         *  background.
         *
         *  @return true if encoding has started, false otherwise.
         *  @exception Exception
         */
        virtual bool
        start ( void )                          throw ( Exception );

        /**
         *  Stop encoding. Stops the encoding running in the background.
         *
         *  @exception Exception
         */
        virtual void
        stop ( void )                           throw ( Exception );
};


/* ================================================= external data structures */


/* ====================================================== function prototypes */



#endif  /* EXTERNAL_ENCODER_H */


/*------------------------------------------------------------------------------
 
  $Source$

  $Log$
  Revision 1.5  2000/11/18 11:13:27  darkeye
  removed direct reference to cout, except from main.cpp
  all class use the Reporter interface to report events

  Revision 1.4  2000/11/12 14:54:50  darkeye
  added kdoc-style documentation comments

  Revision 1.3  2000/11/05 17:37:24  darkeye
  removed clone() functions

  Revision 1.2  2000/11/05 14:08:27  darkeye
  changed builting to an automake / autoconf environment

  Revision 1.1.1.1  2000/11/05 10:05:50  darkeye
  initial version

  
------------------------------------------------------------------------------*/

