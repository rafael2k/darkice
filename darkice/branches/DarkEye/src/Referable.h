/*------------------------------------------------------------------------------

   Copyright (c) 2000 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : Referable.h
   Version  : $Revision$
   Author   : $Author$
   Location : $Source$
   
   Abstract : 

     Base class for an object for which references can be made
     with the reference class Ref (see Ref.h)

     usage:
    
     class A : public virtual Referable
     {
        ...
     };

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
#ifndef REFERABLE_H
#define REFERABLE_H

#ifndef __cplusplus
#error This is a C++ include file
#endif


/* ============================================================ include files */

#include "Exception.h"


/* ================================================================ constants */


/* =================================================================== macros */


/* =============================================================== data types */

/*------------------------------------------------------------------------------
 *  
 *----------------------------------------------------------------------------*/
class Referable
{
    private:
    
        unsigned int    referenceCount;

        static const
        unsigned int    maxCount = ~((unsigned int)0);


    protected:

        inline
        Referable ( void )                              throw ()
        {
            referenceCount = 0;
        }

        
        inline virtual
        ~Referable ( void )                             throw ( Exception )
        {
            if ( referenceCount > 0 ) {
                throw Exception( __FILE__, __LINE__,
                                 "reference count positive in destructor",
                                 referenceCount);
            }
        }


    public:
        
        inline unsigned int
        increaseReferenceCount ( void )                 throw ( Exception )
        {
            if ( referenceCount >= maxCount ) {
                throw Exception( __FILE__,
                                 __LINE__,
                                 "reference count overflow",
                                 referenceCount );
            }
            return ++referenceCount;
        }


        inline unsigned int
        decreaseReferenceCount ( void )                 throw ( Exception )
        {
            if ( referenceCount == 0 ) {
                throw Exception( __FILE__, __LINE__,
                                 "reference count underflow",
                                 referenceCount );
            }
            return --referenceCount;
        }


        inline unsigned int
        getReferenceCount ( void ) const                throw ()
        {
            return referenceCount;
        }
};


/* ================================================= external data structures */


/* ====================================================== function prototypes */



#endif  /* REFERABLE_H */


/*------------------------------------------------------------------------------
 
  $Source$

  $Log$
  Revision 1.1  2000/11/05 10:05:54  darkeye
  Initial revision

  
------------------------------------------------------------------------------*/

