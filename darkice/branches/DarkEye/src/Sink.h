/*------------------------------------------------------------------------------

   Copyright (c) 2000 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : Sink.h
   Version  : $Revision$
   Author   : $Author$
   Location : $Source$
   
   Abstract : 

     A general data sink

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
#ifndef SINK_H
#define SINK_H

#ifndef __cplusplus
#error This is a C++ include file
#endif


/* ============================================================ include files */

#include "Referable.h"


/* ================================================================ constants */


/* =================================================================== macros */


/* =============================================================== data types */

/*------------------------------------------------------------------------------
 *  
 *----------------------------------------------------------------------------*/
class Sink : public virtual Referable
{
    private:


    protected:

        inline
        Sink ( void )
        {
        }


        inline
        Sink (      const Sink &    sink )
        {
        }


        inline virtual Sink &
        operator= ( const Sink &    sink )
        {
            return *this;
        }


    public:

        inline virtual
        ~Sink ( void )
        {
        }


        virtual bool
        open ( void )           = 0;


        virtual bool
        isOpen ( void ) const   = 0;


        virtual bool
        canWrite (              unsigned int    sec,
                                unsigned int    usec )      = 0;


        virtual unsigned int
        write (                 const void    * buf,
                                unsigned int    len )       = 0;


        virtual void
        flush ( void )          = 0;


        virtual void
        close ( void )          = 0;
};


/* ================================================= external data structures */


/* ====================================================== function prototypes */



#endif  /* SINK_H */


/*------------------------------------------------------------------------------
 
  $Source$

  $Log$
  Revision 1.1  2000/11/05 10:05:54  darkeye
  Initial revision

  
------------------------------------------------------------------------------*/

