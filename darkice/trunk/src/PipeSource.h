/*------------------------------------------------------------------------------

   Copyright (c) 2000 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : PipeSource.h
   Version  : $Revision$
   Author   : $Author$
   Location : $Source$
   
   Abstract : 


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

/**
 *  FIFO pipe data input
 *
 *  @author  $Author$
 *  @version $Revision$
 */
class PipeSource : public FileSource
{
    private:


    protected:

        /**
         *  Default constructor. Always throws an Exception.
         *  
         *  @exception Exception
         */
        inline
        PipeSource ( void )                             throw ( Exception )
        {
            throw Exception( __FILE__, __LINE__);
        }


    public:

        /**
         *  Constructor by a file name.
         *
         *  @param name name of the file to be represented by the object.
         *  @exception Exception
         */
        inline
        PipeSource (  const char        * name )        throw ( Exception )
            : FileSource( name )
        {
        }

        /**
         *  Copy constructor.
         *
         *  @param ps the PipeSource to copy.
         *  @exception Exception
         */
        inline
        PipeSource (  const PipeSource &    ps )        throw ( Exception )
            : FileSource( ps )
        {
        }

        /**
         *  Assignment operator.
         *
         *  @param ps the PipeSource to assign to this object.
         *  @return a reference to this object.
         *  @exception Exception
         */
        inline virtual PipeSource &
        operator= ( const PipeSource &    ps )          throw ( Exception )
        {
            if ( this != &ps ) {
                FileSource::operator=( ps );
            }

            return *this;
        }

        /**
         *  Destructor.
         *
         *  @exception Exception
         */
        virtual inline
        ~PipeSource( void )                             throw ( Exception )
        {
        }

        /**
         *  Create the pipe.
         *
         *  @return true if creation was successful, false otherwise.
         *  @exception Exception
         */
        virtual bool
        create ( void )                                 throw ( Exception );
};


/* ================================================= external data structures */


/* ====================================================== function prototypes */



#endif  /* PIPE_SOURCE_H */


/*------------------------------------------------------------------------------
 
  $Source$

  $Log$
  Revision 1.3  2000/11/12 13:31:40  darkeye
  added kdoc-style documentation comments

  Revision 1.2  2000/11/05 17:37:24  darkeye
  removed clone() functions

  Revision 1.1.1.1  2000/11/05 10:05:53  darkeye
  initial version

  
------------------------------------------------------------------------------*/

