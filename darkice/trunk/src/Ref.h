/*------------------------------------------------------------------------------

   Copyright (c) 2000 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : Ref.h
   Version  : $Revision$
   Author   : $Author$
   Location : $Source$
   
   Abstract : 

     Java-like object reference class
     Objects used with this reference class have to be descandents
     of class Referable

     sample usage:

     #include "Ref.h"
     #include "Referable.h"
     
     class  A : public virtual Referable;

     ...
     
     A        * a = new A();
     Ref<A>     ref1 = a;       // 1 reference to a
     Ref<A>     ref2 = ref1;    // 2 references to a

     ref1 = 0;      // 1 reference to a
     ref2 = 0;       // at this point object a is destroyed
    

     Based on Tima Saarinen's work,
     http://gamma.nic.fi/~timosa/comp/refcount.html

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
#ifndef REF_H
#define REF_H

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
template <class T>
class Ref
{
    private:
        
        T* object;


    protected:


    public:

        inline
        Ref ( void )                            throw ()
        {
            object = NULL;
        }


        inline
        Ref ( const Ref<T> &    other )         throw ( Exception )
        {
            object = NULL;
            set( other.object);
        }


        inline
        Ref ( T   * obj )                       throw ( Exception )
        {
            object = obj;
            obj->increaseReferenceCount();
        }

        
        inline virtual
        ~Ref ( void )                           throw ( Exception )
        {
            set( 0 );
        }

        
        inline T*
        operator->() const                      throw ()
        {
            if ( !object ) {
                throw Exception( __FILE__, __LINE__,
                                 "reference to NULL object");
            }
            return object;
        }


        inline Ref<T> &
        operator= ( Ref<T>  other )             throw ( Exception )
        {
            set( other.object);
        }

        
        inline Ref<T> &
        operator= ( T*  obj )                   throw ( Exception )
        {
            set( obj);
            return *this;
        }


        inline void
        set ( T*    newobj )                    throw ( Exception )
        {
            // If equal do nothing
            if ( newobj == object ) {
                return;
            }

            // Increase reference count
            if ( newobj ) {
                newobj->increaseReferenceCount();
            }

            // Decrease the reference count of the old referable
            if ( object ) {
                if ( object->decreaseReferenceCount() == 0 ) {
                    delete object;
                }
            }

            // Assign
            object = newobj;
        }

        
        /**
         * Return object pointer. This method should be used with
         * care because it breaks the encapsulation.
         * Typically this method is needed for the method calls
         * which require literal object pointer.
         *
         * <P>It may not be bad idea to pass the <CODE>Ref</CODE>
         * objects as method arguments.</P>
         *
         * @return Object pointer or <CODE>NULL</CODE>.
         */
        inline T*
        get ( void ) const                      throw ()
        {
            return object;
        }


        inline bool
        operator== ( const Ref<T> &   other ) const     throw ()
        {
            return object == other.object;
        }


        inline bool
        operator!= ( const Ref<T> &   other ) const     throw ()
        {
            return object != other.object;
        }
};

/* ================================================= external data structures */


/* ====================================================== function prototypes */



#endif  /* REF_H */


/*------------------------------------------------------------------------------
 
  $Source$

  $Log$
  Revision 1.1  2000/11/05 10:05:54  darkeye
  Initial revision

  
------------------------------------------------------------------------------*/

