/*------------------------------------------------------------------------------

   Copyright (c) 2000 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : Util.h
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
#ifndef UTIL_H
#define UTIL_H

#ifndef __cplusplus
#error This is a C++ include file
#endif


/* ============================================================ include files */

#include "Exception.h"


/* ================================================================ constants */


/* =================================================================== macros */


/* =============================================================== data types */

/**
 *  Widely used utilities.
 *  This class can not be instantiated, but contains useful (?) static
 *  functions.
 *
 *  Typical usage:
 *
 *  <pre>
 *  #include "Util.h"
 *  
 *  char  * str = Util::strDup( otherStr);
 *  </pre>
 *
 *  @author  $Author$
 *  @version $Revision$
 */
class Util
{
    private:


    protected:

        /**
         *  Default constructor. Always throws an Exception.
         *
         *  @exception Exception
         */
        inline
        Util ( void )                           throw ( Exception )
        {
            throw Exception( __FILE__, __LINE__);
        }

        /**
         *  Copy constructor. Always throws an Exception.
         *
         *  @exception Exception
         */
        inline
        Util ( const Util &   e )               throw ( Exception )
        {
            throw Exception( __FILE__, __LINE__);
        }

        /**
         *  Destructor. Always throws an Exception.
         *
         *  @exception Exception
         */
        inline
        ~Util ( void )                          throw ( Exception )
        {
            throw Exception( __FILE__, __LINE__);
        }

        /**
         *  Assignment operator. Always throws an Exception.
         *
         *  @param u the object to assign to this one.
         *  @exception Exception
         */
        inline Util &
        operator= ( const Util &   u )          throw ( Exception )
        {
            throw Exception( __FILE__, __LINE__);
        }



    public:

        /**
         *  Determine a C string's length.
         *
         *  @param str a zero-terminated C string.
         *  @return length of str
         *  @exception Exception
         */
        static unsigned int
        strLen (        const char    * str )       throw ( Exception );

        /**
         *  Copy a C string into another.
         *
         *  @param dest place for the copy. Storage size must be at least
         *              Util::strLen(src) + 1 long.
         *  @param str the string to copy.
         *  @exception Exception
         */
        static void
        strCpy (    char          * dest,
                    const char    * src )           throw ( Exception );

        /**
         *  Concatenate a string to another's end.
         *
         *  @param dest the string to concatenate to.
         *              Storage size of dest must be at least
         *              Util::strLen(dest) + Util::strLen(src) + 1 long.
         *  @param str the string to concatenate.
         *  @exception Exception
         */
        static void
        strCat (    char          * dest,
                    const char    * src )           throw ( Exception );

        /**
         *  Duplicate a string by allocating space with new[].
         *  The returned string must be freed with delete[].
         *
         *  @param str the string to duplicate.
         *  @exception Exception
         */
        static char *
        strDup (        const char    * str )       throw ( Exception );

        /**
         *  Determine wether two string are equal.
         *
         *  @param str1 one of the strings.
         *  @param str2 the other string.
         *  @return true if the two strings are equal, false othersize.
         *  @exception Exception
         */
        static bool
        strEq ( const char    * str1,
                const char    * str2 )               throw ( Exception );

        /**
         *  Convert a string to long.
         *
         *  @param str the string to convert.
         *  @param base numeric base of number in str.
         *  @return the value of str as a long int
         *  @exception Exception
         */
        static long int
        strToL ( const char    * str,
                 int             base = 10 )         throw ( Exception );
};


/* ================================================= external data structures */


/* ====================================================== function prototypes */



#endif  /* UTIL_H */


/*------------------------------------------------------------------------------
 
  $Source$

  $Log$
  Revision 1.4  2000/11/12 13:31:40  darkeye
  added kdoc-style documentation comments

  Revision 1.3  2000/11/09 22:04:33  darkeye
  added functions strLen strCpy and strCat

  Revision 1.2  2000/11/09 06:44:21  darkeye
  added strEq and strToL functions

  Revision 1.1.1.1  2000/11/05 10:05:55  darkeye
  initial version

  
------------------------------------------------------------------------------*/

