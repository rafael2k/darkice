/*------------------------------------------------------------------------------

   Copyright (c) 2000 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : SolarisDspSource.h
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
#ifndef SOLARIS_DSP_SOURCE_H
#define SOLARIS_DSP_SOURCE_H

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
 *  An audio input based on Solaris /dev/audio devices
 *
 *  @author  $Author$
 *  @version $Revision$
 */
class SolarisDspSource : public AudioSource, public virtual Reporter
{
    private:

        /**
         *  The file name of the OSS DSP device (e.g. /dev/audio)
         */
        char      * fileName;

        /**
         *  The low-level file descriptor of the Solaris DSP device.
         */
        int         fileDescriptor;


    protected:

        /**
         *  Default constructor. Always throws an Exception.
         *
         *  @exception Exception
         */
        inline
        SolarisDspSource ( void )                       throw ( Exception )
        {
            throw Exception( __FILE__, __LINE__);
        }

        /**
         *  Initialize the object
         *
         *  @param name the file name of the Solaris DSP device.
         *  @exception Exception
         */
        void
        init (  const char    * name )              throw ( Exception );

        /**
         *  De-iitialize the object
         *
         *  @exception Exception
         */
        void
        strip ( void )                              throw ( Exception );


    public:

        /**
         *  Constructor.
         *
         *  @param name the file name of the Solaris DSP device
         *              (e.g. /dev/audio or /dev/sound/0)
         *  @param sampleRate samples per second (e.g. 44100 for 44.1kHz).
         *  @param bitsPerSample bits per sample (e.g. 16 bits).
         *  @param channel number of channels of the audio source
         *                 (e.g. 1 for mono, 2 for stereo, etc.).
         *  @exception Exception
         */
        inline
        SolarisDspSource (  const char    * name,
                            int             sampleRate    = 44100,
                            int             bitsPerSample = 16,
                            int             channel       = 2 )
                                                        throw ( Exception )

                    : AudioSource( sampleRate, bitsPerSample, channel)
        {
            init( name);
        }

        /**
         *  Copy Constructor.
         *
         *  @param source the object to copy.
         *  @exception Exception
         */
        inline
        SolarisDspSource (  const SolarisDspSource &    ds )
                                                        throw ( Exception )
                    : AudioSource( ds )
        {
            init( ds.fileName);
        }

        /**
         *  Destructor.
         *
         *  @exception Exception
         */
        inline virtual
        ~SolarisDspSource ( void )                          throw ( Exception )
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
        inline virtual SolarisDspSource &
        operator= (     const SolarisDspSource &     ds )   throw ( Exception )
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
         *  @return true
         */
        virtual inline bool
        isBigEndian ( void ) const           throw ()
        {
#ifdef WORDS_BIGENDIAN
            return true;
#else
            return false;
#endif
        }

        /**
         *  Open the SolarisDspSource.
         *  This does not put the Solaris DSP device into recording mode.
         *  To start getting samples, call either canRead() or read().
         *
         *  @return true if opening was successful, false otherwise
         *  @exception Exception
         *  
         *  @see #canRead
         *  @see #read
         */
        virtual bool
        open ( void )                                   throw ( Exception );

        /**
         *  Check if the SolarisDspSource is open.
         *
         *  @return true if the SolarisDspSource is open, false otherwise.
         */
        inline virtual bool
        isOpen ( void ) const                           throw ()
        {
            return fileDescriptor != 0;
        }

        /**
         *  Check if the SolarisDspSource can be read from.
         *  Blocks until the specified time for data to be available.
         *  Puts the Solaris DSP device into recording mode.
         *
         *  @param sec the maximum seconds to block.
         *  @param usec micro seconds to block after the full seconds.
         *  @return true if the SolarisDspSource is ready to be read from,
         *          false otherwise.
         *  @exception Exception
         */
        virtual bool
        canRead (               unsigned int    sec,
                                unsigned int    usec )  throw ( Exception );

        /**
         *  Read from the SolarisDspSource.
         *  Puts the Solaris DSP device into recording mode.
         *
         *  @param buf the buffer to read into.
         *  @param len the number of bytes to read into buf
         *  @return the number of bytes read (may be less than len).
         *  @exception Exception
         */
        virtual unsigned int
        read (                  void          * buf,
                                unsigned int    len )   throw ( Exception );

        /**
         *  Close the SolarisDspSource.
         *
         *  @exception Exception
         */
        virtual void
        close ( void )                                  throw ( Exception );
};


/* ================================================= external data structures */


/* ====================================================== function prototypes */



#endif  /* SOLARIS_DSP_SOURCE_H */


/*------------------------------------------------------------------------------
 
  $Source$

  $Log$
  Revision 1.4  2004/02/19 06:47:06  darkeye
  finalized OpenBSD port

  Revision 1.3  2004/02/18 21:08:11  darkeye
  ported to OpenBSD (real-time scheduling not yet supported)

  Revision 1.2  2001/09/18 14:57:19  darkeye
  finalized Solaris port

  Revision 1.1  2001/09/11 15:05:21  darkeye
  added Solaris support

  
------------------------------------------------------------------------------*/

