/*------------------------------------------------------------------------------

   Copyright (c) 2000-2007 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : Exception.h
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
#ifndef EXCEPTION_H
#define EXCEPTION_H

#ifndef __cplusplus
#error This is a C++ include file
#endif


/* ============================================================ include files */

#include <iostream>


/* ================================================================ constants */


/* =================================================================== macros */


/* =============================================================== data types */

/**
 *  An exception class.
 *
 *  This class should not depend on any other class
 *  should not throw any exceptions itself.
 *
 *  Typical usage:
 *
 *  <pre>
 *  throw Exception( __FILE__, __LINE__, "describe the exception", code);
 *  </pre>
 *
 *  @author  $Author: rafael@riseup.net $
 *  @version $Revision: 474 $
 */
class Exception
{
    private:

        /**
         *  Source file the exception was thrown in.
         */
        char          * file;

        /**
         *  Line number in the source file the exception was thrown in.
         */
        unsigned int    line;

        /**
         *  Textual description of the exception.
         */
        char          * description;

        /**
         *  Numerical error code.
         */
        int             code;

        /**
         *  Initalize the object.
         *
         *  @param file the source file the exception was thrown in.
         *  @param line the line in the source file.
         *  @param description textual description of the exception.
         *  @param code numerical error code.
         */
        void
        init (  const char            * file,
                unsigned int            line,
                const char            * description,
                int                     code )              throw ();

        /**
         *  De-initalize the object.
         */
        void
        strip ()                                            throw ();


    protected:


    public:

        /**
         *  Default constructor.
         */
        inline
        Exception ( void )                                  throw ()
        {
            init( 0, 0, 0, 0);
        }

        /**
         *  Copy constructor.
         */
        inline
        Exception ( const Exception &   e )                 throw ()
        {
            init( e.file, e.line, e.description, e.code);
        }

        /**
         *  Construct by a description and error code.
         *
         *  @param description textual description of the exception.
         *  @param code numerical error code.
         */
        inline
        Exception ( const char            * description,
                    int                     code = 0 )      throw ()
        {
            init( 0, 0, description, code);
        }

        /**
         *  Construct by source file information, a description and error code.
         *
         *  @param file the source file the exception was thrown in.
         *  @param line the line in the source file.
         *  @param description textual description of the exception.
         *  @param code numerical error code.
         */
        inline
        Exception ( const char            * file,
                    unsigned int            line,
                    const char            * description  = 0,
                    int                     code = 0 )          throw ()
        {
            init( file, line, description, code);
        }

        /**
         *  Construct by source file information, a description and error code.
         *  The description is constructed from two strings, any of which
         *  may be NULL.
         *
         *  @param file the source file the exception was thrown in.
         *  @param line the line in the source file.
         *  @param description1 textual description of the exception part 1.
         *  @param description2 textual description of the exception part 2.
         *  @param code numerical error code.
         */
        Exception ( const char            * file,
                    unsigned int            line,
                    const char            * description1,
                    const char            * description2,
                    int                     code = 0 )          throw ();

        /**
         *  Construct by source file information, a description and error code.
         *  The description is constructed from three strings, any of
         *  which may be NULL.
         *
         *  @param file the source file the exception was thrown in.
         *  @param line the line in the source file.
         *  @param description1 textual description of the exception part 1.
         *  @param description2 textual description of the exception part 2.
         *  @param description3 textual description of the exception part 3.
         *  @param code numerical error code.
         */
        Exception ( const char            * file,
                    unsigned int            line,
                    const char            * description1,
                    const char            * description2,
                    const char            * description3,
                    int                     code = 0 )          throw ();

        /**
         *  Desctructor.
         */
        inline
        ~Exception ( void )                                 throw ()
        {
            strip();
        }

        /**
         *  Assignment operator.
         *
         *  @param e the Exception to assign this to.
         *  @return a reference to this Exception.
         */
        inline Exception &
        operator= ( const Exception &   e )                 throw ()
        {
            if ( this != &e ) {
                strip();
                init( e.file, e.line, e.description, e.code);
            }

            return *this;
        }

        /**
         *  Return the textual description of the Exception.
         *
         *  @return the textual description of the Exception.
         */
        inline const char *
        getDescription( void ) const                        throw ()
        {
            return description;
        }

        /**
         *  Return the line number in the source file this Exception was
         *  thrown in.
         *
         *  @return the line number in the source file this Exception was
         *          thrown in.
         */
        inline unsigned int
        getLine ( void ) const                              throw ()
        {
            return line;
        }

        /**
         *  Return the source file this Exception was thrown in.
         *
         *  @return the source file this Exception was thrown in.
         */
        inline const char *
        getFile ( void ) const                              throw ()
        {
            return file;
        }

        /**
         *  Return the numerical code of the Exception.
         *
         *  @return the numerical code of the Exception.
         */
        inline int
        getCode ( void ) const                              throw ()
        {
            return code;
        }
};


/* ================================================= external data structures */


/* ====================================================== function prototypes */

/**
 *  Print an Exception to an ostream.
 *
 *  @param os the output stream to print to.
 *  @param e the Exception to print.
 *  @return a reference to the supplied output stream.
 */
inline std::ostream &
operator<< (        std::ostream &      os,
                    const Exception &   e )
{
    os << e.getFile() << ":" << e.getLine() << ": "
       << e.getDescription() << " [" << e.getCode() << "]";

    return os;
}



#endif  /* EXCEPTION_H */

