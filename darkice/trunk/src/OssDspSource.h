/*------------------------------------------------------------------------------

   Copyright (c) 2000 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : OssDspSource.h
   Version  : $Revision$
   Author   : $Author$
   Location : $Source$
   
   Abstract : 

     Audio data input from an OSS /dev/dsp-like device

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
#ifndef OSS_DSP_SOURCE_H
#define OSS_DSP_SOURCE_H

#ifndef __cplusplus
#error This is a C++ include file
#endif


/* ============================================================ include files */

#include "AudioSource.h"


/* ================================================================ constants */


/* =================================================================== macros */


/* =============================================================== data types */

/*------------------------------------------------------------------------------
 *  An audio input based on /dev/dsp-like raw devices
 *----------------------------------------------------------------------------*/
class OssDspSource : public AudioSource
{
    private:

        char      * fileName;
        int         fileDescriptor;
        bool        running;


    protected:

        inline
        OssDspSource ( void )                       throw ( Exception )
        {
            throw Exception( __FILE__, __LINE__);
        }


        void
        init (  const char    * name )              throw ( Exception );


        void
        strip ( void )                              throw ( Exception );


    public:

        inline
        OssDspSource (  const char    * name,
                        int             sampleRate    = 44100,
                        int             bitsPerSample = 16,
                        int             channel       = 2 )
                                                        throw ( Exception )

                    : AudioSource( sampleRate, bitsPerSample, channel)
        {
            init( name);
        }


        inline
        OssDspSource (  const OssDspSource &    ds )    throw ( Exception )
                    : AudioSource( ds )
        {
            init( ds.fileName);
        }


        inline virtual
        ~OssDspSource ( void )                          throw ( Exception )
        {
            strip();
        }


        inline virtual OssDspSource &
        operator= (     const OssDspSource &     ds )   throw ( Exception )
        {
            if ( this != &ds ) {
                strip();
                AudioSource::operator=( ds);
                init( ds.fileName);
            }
            return *this;
        }


        inline virtual OssDspSource *
        clone ( void ) const                            throw ( Exception )
        {
            return new OssDspSource( *this );
        }


        virtual bool
        open ( void )                                   throw ( Exception );


        inline virtual bool
        isOpen ( void ) const                           throw ()
        {
            return fileDescriptor != 0;
        }


        virtual bool
        canRead (               unsigned int    sec,
                                unsigned int    usec )  throw ( Exception );


        virtual unsigned int
        read (                  void          * buf,
                                unsigned int    len )   throw ( Exception );


        virtual void
        close ( void )                                  throw ( Exception );
};


/* ================================================= external data structures */


/* ====================================================== function prototypes */



#endif  /* OSS_DSP_SOURCE_H */


/*------------------------------------------------------------------------------
 
  $Source$

  $Log$
  Revision 1.1  2000/11/05 10:05:53  darkeye
  Initial revision

  
------------------------------------------------------------------------------*/

