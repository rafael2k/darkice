/*------------------------------------------------------------------------------

   Copyright (c) 2000 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : PipeSource.h
   Version  : $Revision$
   Author   : $Author$
   Location : $Source$
   
   Abstract : 

     FIFO pipe data input

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
#ifndef PIPE_SOURCE_H
#define PIPE_SOURCE_H

#ifndef __cplusplus
#error This is a C++ include file
#endif


/* ============================================================ include files */

#include "FileSource.h"


/* ================================================================ constants */


/* =================================================================== macros */


/* =============================================================== data types */

/*------------------------------------------------------------------------------
 *  
 *----------------------------------------------------------------------------*/
class PipeSource : public FileSource
{
    private:


    protected:

        inline
        PipeSource ( void )                             throw ( Exception )
        {
            throw Exception( __FILE__, __LINE__);
        }


    public:

        inline
        PipeSource (  const char        * name )        throw ( Exception )
            : FileSource( name )
        {
        }


        inline
        PipeSource (  const PipeSource &    ps )        throw ( Exception )
            : FileSource( ps )
        {
        }


        inline virtual PipeSource &
        operator= ( const PipeSource &    fs )          throw ( Exception )
        {
            if ( this != &fs ) {
                FileSource::operator=( fs );
            }

            return *this;
        }


        virtual inline
        ~PipeSource( void )                             throw ( Exception )
        {
        }


        virtual bool
        create ( void )                                 throw ( Exception );
};


/* ================================================= external data structures */


/* ====================================================== function prototypes */



#endif  /* PIPE_SOURCE_H */


/*------------------------------------------------------------------------------
 
  $Source$

  $Log$
  Revision 1.2  2000/11/05 17:37:24  darkeye
  removed clone() functions

  Revision 1.1.1.1  2000/11/05 10:05:53  darkeye
  initial version

  
------------------------------------------------------------------------------*/

