/*------------------------------------------------------------------------------

   Copyright (c) 2000 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : FileSource.h
   Version  : $Revision$
   Author   : $Author$
   Location : $Source$
   
   Abstract : 

     Data input from a file

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
#ifndef FILE_SOURCE_H
#define FILE_SOURCE_H

#ifndef __cplusplus
#error This is a C++ include file
#endif


/* ============================================================ include files */

#include "Source.h"


/* ================================================================ constants */


/* =================================================================== macros */


/* =============================================================== data types */

/*------------------------------------------------------------------------------
 *  A data source based on a file
 *----------------------------------------------------------------------------*/
class FileSource : public Source
{
    private:

        char      * fileName;
        int         fileDescriptor;


        void
        init (  const char        * name )              throw ( Exception );


        void
        strip ( void )                                  throw ( Exception );


    protected:

        inline
        FileSource ( void )                             throw ( Exception )
        {
            throw Exception( __FILE__, __LINE__);
        }


    public:

        inline
        FileSource (    const char        * name )      throw ( Exception )
        {
            init( name);
        }


        FileSource (    const FileSource &  fs )        throw ( Exception );


        inline
        ~FileSource ( void )                            throw ( Exception )
        {
            strip();
        }


        inline virtual FileSource &
        operator= (     const FileSource &  fs )        throw ( Exception );


        inline virtual const char *
        getFileName ( void ) const                      throw ()
        {
            return fileName;
        }


        virtual bool
        exists ( void ) const                           throw ();


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



#endif  /* FILE_SOURCE_H */


/*------------------------------------------------------------------------------
 
  $Source$

  $Log$
  Revision 1.1  2000/11/05 10:05:51  darkeye
  Initial revision

  
------------------------------------------------------------------------------*/

