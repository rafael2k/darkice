/*------------------------------------------------------------------------------

   Copyright (c) 2000-2007 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : Referable.h
   Version  : $Revision: 474 $
   Author   : $Author: rafael@riseup.net $
   Location : $HeadURL$
   
   Copyright notice:

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License  
    as published by the Free Software Foundation; either version 3
    of the License, or (at your option) any later version.
   
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of 
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
    GNU General Public License for more details.
   
    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

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

/**
 *  Base class for an object for which references can be made
 *  with the reference class Ref.
 *
 *  usage:
 * 
 *  <pre>
 *  class A : public virtual Referable
 *  {
 *     ...
 *  };
 *  </pre>
 *  
 *  @ref Ref
 *
 *  @author  $Author: rafael@riseup.net $
 *  @version $Revision: 474 $
 */
class Referable
{
    private:

        /**
         *  Number of references to the object.
         */
        unsigned int    referenceCount;

        /**
         *  Maximum number of references before an overflow occurs.
         */
        static const
        unsigned int    maxCount = ~((unsigned int)0);


    protected:

        /**
         *  Default constructor.
         */
        inline
        Referable ( void )                              throw ()
        {
            referenceCount = 0;
        }

        
        /**
         *  Desctructor.
         *
         *  @exception Exception
         */
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
        
        /**
         *  Increase reference count.
         *
         *  @return the new reference count.
         *  @exception Exception
         */
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

        /**
         *  Decrease reference count.
         *
         *  @return the new reference count.
         *  @exception Exception
         */
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

        /**
         *  Get the reference count.
         *
         *  @return the reference count.
         */
        inline unsigned int
        getReferenceCount ( void ) const                throw ()
        {
            return referenceCount;
        }
};


/* ================================================= external data structures */


/* ====================================================== function prototypes */



#endif  /* REFERABLE_H */

