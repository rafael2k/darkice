/*------------------------------------------------------------------------------

   Copyright (c) 2000 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : PipeSink.h
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
#ifndef PIPE_SINK_H
#define PIPE_SINK_H

#ifndef __cplusplus
#error This is a C++ include file
#endif


/* ============================================================ include files */

#include "FileSink.h"


/* ================================================================ constants */


/* =================================================================== macros */


/* =============================================================== data types */

/**
 *  FIFO pipe data output
 *
 *  @author  $Author$
 *  @version $Revision$
 */
class PipeSink : public FileSink
{
    private:


    protected:

        /**
         *  Default constructor. Always throws an Exception.
         *  
         *  @exception Exception
         */
        inline
        PipeSink ( void )                               throw ( Exception )
        {
            throw Exception( __FILE__, __LINE__);
        }


    public:

        /**
         *  Constructor by a pipe name.
         *
         *  @param name name of the pipe to be represented by the object.
         *  @exception Exception
         */
        inline
        PipeSink (  const char        * name )          throw ( Exception )
            : FileSink( name )
        {
        }

        /**
         *  Copy constructor.
         *
         *  @param fsink the PipeSink to copy.
         *  @exception Exception
         */
        inline
        PipeSink (  const PipeSink &    ps )            throw ( Exception )
            : FileSink( ps )
        {
        }

        /**
         *  Assignment operator.
         *
         *  @param ps the PipeSink to assign to this object.
         *  @return a reference to this object.
         *  @exception Exception
         */
        inline virtual PipeSink &
        operator= ( const PipeSink &    ps )            throw ( Exception )
        {
            if ( this != &ps ) {
                FileSink::operator=( ps );
            }

            return *this;
        }

        /**
         *  Destructor.
         *
         *  @exception Exception
         */
        virtual inline
        ~PipeSink( void )                               throw ( Exception )
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

        /**
         *  Open the pipe.
         *
         *  @return true if opening was successful, false otherwise.
         *  @exception Exception
         */
        virtual bool
        open ( void )                                   throw ( Exception );
};


/* ================================================= external data structures */


/* ====================================================== function prototypes */



#endif  /* PIPE_SINK_H */


/*------------------------------------------------------------------------------
 
  $Source$

  $Log$
  Revision 1.3  2000/11/11 12:33:13  darkeye
  added kdoc-style documentation

  Revision 1.2  2000/11/05 17:37:24  darkeye
  removed clone() functions

  Revision 1.1.1.1  2000/11/05 10:05:53  darkeye
  initial version

  
------------------------------------------------------------------------------*/

