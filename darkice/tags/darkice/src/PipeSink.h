/*------------------------------------------------------------------------------

   Copyright (c) 2000 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : PipeSink.h
   Version  : $Revision$
   Author   : $Author$
   Location : $Source$
   
   Abstract : 

     FIFO pipe data output

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

/*------------------------------------------------------------------------------
 *  
 *----------------------------------------------------------------------------*/
class PipeSink : public FileSink
{
    private:


    protected:

        inline
        PipeSink ( void )                               throw ( Exception )
        {
            throw Exception( __FILE__, __LINE__);
        }


    public:

        inline
        PipeSink (  const char        * name )          throw ( Exception )
            : FileSink( name )
        {
        }


        inline
        PipeSink (  const PipeSink &    ps )            throw ( Exception )
            : FileSink( ps )
        {
        }


        inline virtual PipeSink &
        operator= ( const PipeSink &    fs )            throw ( Exception )
        {
            if ( this != &fs ) {
                FileSink::operator=( fs );
            }

            return *this;
        }


        inline virtual PipeSink *
        clone ( void ) const                            throw ( Exception )
        {
            return new PipeSink(*this);
        }


        virtual inline
        ~PipeSink( void )                               throw ( Exception )
        {
        }


        virtual bool
        create ( void )                                 throw ( Exception );


        virtual bool
        open ( void )                                   throw ( Exception );
};


/* ================================================= external data structures */


/* ====================================================== function prototypes */



#endif  /* PIPE_SINK_H */


/*------------------------------------------------------------------------------
 
  $Source$

  $Log$
  Revision 1.1  2000/11/05 10:05:53  darkeye
  Initial revision

  
------------------------------------------------------------------------------*/

