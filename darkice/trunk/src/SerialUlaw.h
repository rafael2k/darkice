/*------------------------------------------------------------------------------

   Copyright (c) 2000-2007 Tyrell Corporation. All rights reserved.
   Copyright (c) 2007 Clyde Stubbs

   Tyrell DarkIce

   File     : SerialUlaw.h
   Version  : $Revision$
   Author   : $Author$
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
#ifndef SERIAL_ULAW_SOURCE_H
#define SERIAL_ULAW_SOURCE_H

#ifndef __cplusplus
#error This is a C++ include file
#endif


/* ============================================================ include files */

#include "Reporter.h"
#include "AudioSource.h"


/* ================================================================ constants */


/* =================================================================== macros */


/* =============================================================== data types */

/**
 *  An audio input based on /dev/dsp-like raw devices
 *
 *  @author  $Author$
 *  @version $Revision$
 */
class SerialUlaw : public AudioSource, public virtual Reporter
{
    private:

        /**
         *  The file name of the OSS DSP device (e.g. /dev/dsp or /dev/dsp0).
         */
        char      * fileName;

        /**
         *  The low-level file descriptor of the OSS DSP device.
         */
        int         fileDescriptor;

        /**
         *  Indicates whether the low-level OSS DSP device is in a recording
         *  state.
         */
        bool        running;


    protected:

        /**
         *  Default constructor. Always throws an Exception.
         *
         *  @exception Exception
         */
        inline
        SerialUlaw ( void )                       
        {
            throw Exception( __FILE__, __LINE__);
        }

        /**
         *  Initialize the object
         *
         *  @param name the file name of the OSS DSP device.
         *  @exception Exception
         */
        void
        init (  const char    * name )              ;

        /**
         *  De-iitialize the object
         *
         *  @exception Exception
         */
        void
        strip ( void )                              ;


    public:

        /**
         *  Constructor.
         *
         *  @param name the file name of the OSS DSP device
         *              (e.g. /dev/dsp or /dev/dsp0).
         *  @param sampleRate samples per second (e.g. 44100 for 44.1kHz).
         *  @param bitsPerSample bits per sample (e.g. 16 bits).
         *  @param channel number of channels of the audio source
         *                 (e.g. 1 for mono, 2 for stereo, etc.).
         *  @exception Exception
         */
        inline
        SerialUlaw (  const char    * name,
                      int             sampleRate    = 44100,
                      int             bitsPerSample = 16,
                      int             channel       = 2 )
                                                        

                    : AudioSource( sampleRate, bitsPerSample, channel)
        {
            init( name);
        }

        /**
         *  Copy Constructor.
         *
         *  @param ods the object to copy.
         *  @exception Exception
         */
        inline
        SerialUlaw (  const SerialUlaw &    ods )   
                    : AudioSource( ods )
        {
            init( ods.fileName);
        }

        /**
         *  Destructor.
         *
         *  @exception Exception
         */
        inline virtual
        ~SerialUlaw ( void )                          
        {
            strip();
        }

        /**
         *  Assignment operator.
         *
         *  @param ds the object to assign to this one.
         *  @return a reference to this object.
         *  @exception Exception
         */
        inline virtual SerialUlaw &
        operator= (     const SerialUlaw &     ds )   
        {
            if ( this != &ds ) {
                strip();
                AudioSource::operator=( ds);
                init( ds.fileName);
            }
            return *this;
        }

        /**
         *  Tell if the data from this source comes in big or little endian.
         *
         *  @return true if the source is big endian, false otherwise
         */
        virtual bool
        isBigEndian ( void ) const           throw ();

        /**
         *  Open the SerialUlaw.
         *  This does not put the OSS DSP device into recording mode.
         *  To start getting samples, call either canRead() or read().
         *
         *  @return true if opening was successful, false otherwise
         *  @exception Exception
         *  
         *  @see #canRead
         *  @see #read
         */
        virtual bool
        open ( void )                                   ;

        /**
         *  Check if the SerialUlaw is open.
         *
         *  @return true if the SerialUlaw is open, false otherwise.
         */
        inline virtual bool
        isOpen ( void ) const                           throw ()
        {
            return fileDescriptor != 0;
        }

        /**
         *  Check if the SerialUlaw can be read from.
         *  Blocks until the specified time for data to be available.
         *  Puts the OSS DSP device into recording mode.
         *
         *  @param sec the maximum seconds to block.
         *  @param usec micro seconds to block after the full seconds.
         *  @return true if the SerialUlaw is ready to be read from,
         *          false otherwise.
         *  @exception Exception
         */
        virtual bool
        canRead (               unsigned int    sec,
                                unsigned int    usec )  ;

        /**
         *  Read from the SerialUlaw.
         *  Puts the OSS DSP device into recording mode.
         *
         *  @param buf the buffer to read into.
         *  @param len the number of bytes to read into buf
         *  @return the number of bytes read (may be less than len).
         *  @exception Exception
         */
        virtual unsigned int
        read (                  void          * buf,
                                unsigned int    len )   ;

        /**
         *  Close the SerialUlaw.
         *
         *  @exception Exception
         */
        virtual void
        close ( void )                                  ;
};


/* ================================================= external data structures */


/* ====================================================== function prototypes */



#endif  /* SERIAL_ULAW_SOURCE_H */
