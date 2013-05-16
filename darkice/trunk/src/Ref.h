/*------------------------------------------------------------------------------

   Copyright (c) 2000-2007 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : Ref.h
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

/**
 *  Java-like object reference class.
 *  Objects used with this reference class have to be descandents
 *  of class Referable.
 *
 *  sample usage:
 *
 *  <pre>
 *  #include "Ref.h"
 *  #include "Referable.h"
 *
 *  class  A : public virtual Referable;
 *
 *  ...
 *   
 *  A        * a = new A();
 *  Ref<A>     ref1 = a;       // 1 reference to a
 *  Ref<A>     ref2 = ref1;    // 2 references to a
 *
 *  ref1 = 0;      // 1 reference to a
 *  ref2 = 0;      // at this point object a is destroyed
 *  </pre>
 *
 *  Based on Tima Saarinen's work,
 *  http://gamma.nic.fi/~timosa/comp/refcount.html
 *
 *  @ref Referable
 *  
 *  @author  $Author: rafael@riseup.net $
 *  @version $Revision: 474 $
 */
template <class T>
class Ref
{
    private:
        
        /**
         *  The object referenced by this Ref.
         *  Must be a descandant of Referable.
         */
        T* object;


    protected:


    public:

        /**
         *  Default constructor.
         */
        inline
        Ref ( void )                            throw ()
        {
            object = NULL;
        }

        /**
         *  Copy constructor.
         *
         *  @param other the Ref to copy.
         *  @exception Exception
         */
        inline
        Ref ( const Ref<T> &    other )         throw ( Exception )
        {
            object = NULL;
            set( other.object);
        }

        /**
         *  Constructor based on an object to reference.
         *
         *  @param obj the object to reference.
         *  @exception Exception
         */
        inline
        Ref ( T   * obj )                       throw ( Exception )
        {
            object = obj;
            obj->increaseReferenceCount();
        }

        /**
         *  Destructor.
         *
         *  @exception Exception
         */
        inline virtual
        ~Ref ( void )                           throw ( Exception )
        {
            set( 0 );
        }

        /**
         *  Operator overload to make the reference seem like a pointer.
         *
         *  @return the pointer to the object referenced.
         */
        inline T*
        operator->() const                      throw ( Exception )
        {
            if ( !object ) {
                throw Exception( __FILE__, __LINE__,
                                 "reference to NULL object");
            }
            return object;
        }

        /**
         *  Assignment operator.
         *
         *  @param other the Ref to assign to this one.
         *  @return a reference to this Ref.
         *  @exception Exception
         */
        inline Ref<T> &
        operator= ( Ref<T>  other )             throw ( Exception )
        {
            set( other.object);
            return *this;
        }

        /**
         *  Assignment operator.
         *
         *  @param obj pointer to the object to assign to this Ref.
         *  @return a reference to this Ref.
         *  @exception Exception
         */
        inline Ref<T> &
        operator= ( T*  obj )                   throw ( Exception )
        {
            set( obj);
            return *this;
        }

        /**
         *  Set the object referenced.
         *  Deletes the old referenced object is this was it's last reference.
         *
         *  @param newobj pointer to the object to reference by this Ref.
         *  @exception Exception
         */
        inline void
        set ( T   * newobj )                    throw ( Exception )
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
         *  Return object pointer. This method should be used with
         *  care because it breaks the encapsulation.
         *  Typically this method is needed for the method calls
         *  which require literal object pointer.
         *
         *  It may not be bad idea to pass the Ref
         *  objects as method arguments.
         *
         *  @return Object pointer or NULL.
         */
        inline T*
        get ( void ) const                      throw ()
        {
            return object;
        }

        /**
         *  Equality operator.
         *
         *  @param other the pointer to compare this with.
         *  @return true is this Ref refers to the same object as other,
         *          false otherwise.
         */
        inline bool
        operator== ( const T        * other ) const     throw ()
        {
            return object == other;
        }

        /**
         *  Equality operator.
         *
         *  @param other the Ref to compare this with.
         *  @return true is the two Refs refer to the same object,
         *          false otherwise.
         */
        inline bool
        operator== ( const Ref<T> &   other ) const     throw ()
        {
            return object == other.object;
        }

        /**
         *  Unequality operator.
         *
         *  @param other the pointer to compare this with.
         *  @return false is this Ref refers to a different object then other,
         *          true otherwise.
         */
        inline bool
        operator!= ( const T        * other ) const     throw ()
        {
            return object != other;
        }

        /**
         *  Unequality operator.
         *
         *  @param other the Ref to compare this with.
         *  @return false is the two Refs refer to the same object,
         *          true otherwise.
         */
        inline bool
        operator!= ( const Ref<T> &   other ) const     throw ()
        {
            return object != other.object;
        }
};

/* ================================================= external data structures */


/* ====================================================== function prototypes */



#endif  /* REF_H */

