/*------------------------------------------------------------------------------

   Copyright (c) 2000 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : Exception.h
   Version  : $Revision$
   Author   : $Author$
   Location : $Source$
   
   Abstract : 

     An exception class

     This class should not depend on any other class
     (note: Cloneable is an interface) and should not throw
     any exceptions itself

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
#ifndef EXCEPTION_H
#define EXCEPTION_H

#ifndef __cplusplus
#error This is a C++ include file
#endif


/* ============================================================ include files */

#include <iostream.h>


/* ================================================================ constants */


/* =================================================================== macros */


/* =============================================================== data types */

/*------------------------------------------------------------------------------
 *  
 *----------------------------------------------------------------------------*/
class Exception
{
    private:

        char          * file;
        unsigned int    line;
        char          * description;
        int             code;


        void
        init (  const char            * file,
                unsigned int            line,
                const char            * description,
                int                     code )              throw ();


        void
        strip ()                                            throw ();


    protected:


    public:

        inline
        Exception ( void )                                  throw ()
        {
            init( 0, 0, 0, 0);
        }


        inline
        Exception ( const Exception &   e )                 throw ()
        {
            init( e.file, e.line, e.description, e.code);
        }


        inline
        Exception ( const char            * description,
                    int                     code = 0 )      throw ()
        {
            init( 0, 0, description, code);
        }


        inline
        Exception ( const char            * file,
                    unsigned int            line,
                    const char            * description  = 0,
                    int                     code = 0 )          throw ()
        {
            init( file, line, description, code);
        }


        Exception ( const char            * file,
                    unsigned int            line,
                    const char            * description1,
                    const char            * description2,
                    int                     code = 0 )          throw ();


        Exception ( const char            * file,
                    unsigned int            line,
                    const char            * description1,
                    const char            * description2,
                    const char            * description3,
                    int                     code = 0 )          throw ();


        inline
        ~Exception ( void )                                 throw ()
        {
            strip();
        }


        inline Exception &
        operator= ( const Exception &   e )                 throw ()
        {
            if ( this != &e ) {
                strip();
                init( e.file, e.line, e.description, e.code);
            }

            return *this;
        }


        inline const char *
        getDescription( void ) const                        throw ()
        {
            return description;
        }

        inline unsigned int
        getLine ( void ) const                              throw ()
        {
            return line;
        }


        inline const char *
        getFile ( void ) const                              throw ()
        {
            return file;
        }


        inline int
        getCode ( void ) const                              throw ()
        {
            return code;
        }

};


/* ================================================= external data structures */


/* ====================================================== function prototypes */

/*------------------------------------------------------------------------------
 *  Print a Exception to an ostream
 *----------------------------------------------------------------------------*/
inline ostream &
operator<< (        ostream &           os,
                    const Exception &   e )
{
    os << e.getFile() << ":" << e.getLine() << ": "
       << e.getDescription() << " [" << e.getCode() << "]";
}



#endif  /* EXCEPTION_H */


/*------------------------------------------------------------------------------
 
  $Source$

  $Log$
  Revision 1.3  2000/11/09 22:05:44  darkeye
  added multiple-string constructors

  Revision 1.2  2000/11/05 14:08:27  darkeye
  changed builting to an automake / autoconf environment

  Revision 1.1.1.1  2000/11/05 10:05:50  darkeye
  initial version

  
------------------------------------------------------------------------------*/

