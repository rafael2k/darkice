/*------------------------------------------------------------------------------

   Copyright (c) 2000 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : Util.h
   Version  : $Revision$
   Author   : $Author$
   Location : $Source$
   
   Abstract : 

     Widely used utilities

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

/*------------------------------------------------------------------------------
 *  
 *----------------------------------------------------------------------------*/
class Util
{
    private:


    protected:

        inline
        Util ( void )                           throw ( Exception )
        {
            throw Exception( __FILE__, __LINE__);
        }


        inline
        Util ( const Util &   e )               throw ( Exception )
        {
            throw Exception( __FILE__, __LINE__);
        }


        inline
        ~Util ( void )                          throw ( Exception )
        {
            throw Exception( __FILE__, __LINE__);
        }


        inline Util &
        operator= ( const Util &   e )          throw ( Exception )
        {
            throw Exception( __FILE__, __LINE__);
        }



    public:

        static char *
        strDup(         const char    * str )       throw ( Exception );


        static bool
        strEq(  const char    * str1,
                const char    * str2 )               throw ( Exception );


        static long int
        strToL(  const char    * str,
                 int             base = 10 )         throw ( Exception );
};


/* ================================================= external data structures */


/* ====================================================== function prototypes */



#endif  /* UTIL_H */


/*------------------------------------------------------------------------------
 
  $Source$

  $Log$
  Revision 1.2  2000/11/09 06:44:21  darkeye
  added strEq and strToL functions

  Revision 1.1.1.1  2000/11/05 10:05:55  darkeye
  initial version

  
------------------------------------------------------------------------------*/

