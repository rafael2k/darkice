/*------------------------------------------------------------------------------

   Copyright (c) 2000 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : Source.h
   Version  : $Revision$
   Author   : $Author$
   Location : $Source$
   
   Abstract : 

     A general data source

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
#ifndef SOURCE_H
#define SOURCE_H

#ifndef __cplusplus
#error This is a C++ include file
#endif


/* ============================================================ include files */

#include "Referable.h"
#include "Exception.h"


/* ================================================================ constants */


/* =================================================================== macros */


/* =============================================================== data types */

/*------------------------------------------------------------------------------
 *  
 *----------------------------------------------------------------------------*/
class Source : public virtual Referable
{
    private:

    protected:

        inline
        Source ( void )                             throw ( Exception )
        {
        }


        inline
        Source (    const Source &  source )        throw ( Exception )
        {
        }


        inline virtual Source &
        operator= ( const Source &  source )        throw ( Exception )
        {
            return *this;
        }


    public:

        inline virtual
        ~Source ( void )                            throw ( Exception )
        {
        }


        virtual bool
        open ( void )                           throw ( Exception )     = 0;


        virtual bool
        isOpen ( void ) const                   throw ()                = 0;


        virtual bool
        canRead (  unsigned int    sec,
                   unsigned int    usec )       throw ( Exception )     = 0;


        virtual unsigned int
        read (     void          * buf,
                   unsigned int    len )        throw ( Exception )     = 0;


        virtual void
        close ( void )                          throw ( Exception )     = 0;
};


/* ================================================= external data structures */


/* ====================================================== function prototypes */



#endif  /* SOURCE_H */


/*------------------------------------------------------------------------------
 
  $Source$

  $Log$
  Revision 1.1  2000/11/05 10:05:54  darkeye
  Initial revision

  
------------------------------------------------------------------------------*/

