/*------------------------------------------------------------------------------

   Copyright (c) 2000 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : FileSink.h
   Version  : $Revision$
   Author   : $Author$
   Location : $Source$
   
   Abstract : 

     File data output

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
#ifndef FILE_SINK_H
#define FILE_SINK_H

#ifndef __cplusplus
#error This is a C++ include file
#endif


/* ============================================================ include files */

#include "Sink.h"


/* ================================================================ constants */


/* =================================================================== macros */


/* =============================================================== data types */

/*------------------------------------------------------------------------------
 *  
 *----------------------------------------------------------------------------*/
class FileSink : public Sink
{
    private:

        char      * fileName;


        void
        init (  const char    * name )              throw ( Exception );


        void
        strip ( void )                              throw ( Exception );


    protected:

        int         fileDescriptor;


        inline
        FileSink ( void )                           throw ( Exception )
        {
            throw Exception( __FILE__, __LINE__);
        }


    public:

        inline
        FileSink(   const char        * name )      throw ( Exception )
        {
            init( name);
        }


        FileSink(   const FileSink &    fsink )     throw ( Exception );


        inline virtual
        ~FileSink( void )                           throw ( Exception )
        {
            strip();
        }


        virtual FileSink &
        operator= ( const FileSink &    fs )        throw ( Exception );


        inline virtual FileSink *
        clone ( void ) const                        throw ( Exception )
        {
            return new FileSink(*this);
        }


        inline const char *
        getFileName ( void ) const                  throw ()
        {
            return fileName;
        }


        virtual bool
        exists ( void ) const                       throw ();


        virtual bool
        create ( void )                             throw ( Exception );


        virtual bool
        open ( void )                               throw ( Exception );


        inline virtual bool
        isOpen ( void ) const                       throw ()
        {
            return fileDescriptor != 0;
        }


        virtual bool
        canWrite (     unsigned int    sec,
                       unsigned int    usec )       throw ( Exception );


        virtual unsigned int
        write (        const void    * buf,
                       unsigned int    len )        throw ( Exception );


        inline virtual void
        flush ( void )                              throw ( Exception )
        {
        }


        virtual void
        close ( void )                              throw ( Exception );
};


/* ================================================= external data structures */


/* ====================================================== function prototypes */



#endif  /* FILE_SINK_H */


/*------------------------------------------------------------------------------
 
  $Source$

  $Log$
  Revision 1.1  2000/11/05 10:05:51  darkeye
  Initial revision

  
------------------------------------------------------------------------------*/

