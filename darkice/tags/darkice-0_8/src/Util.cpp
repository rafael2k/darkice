/*------------------------------------------------------------------------------

   Copyright (c) 2000 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : Util.cpp
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

/* ============================================================ include files */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#else
#error need string.h
#endif

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#else
#error need stdlib.h
#endif

#ifdef HAVE_LIMITS_H
#include <limits.h>
#else
#error need limits.h
#endif


#include "Util.h"


/* ===================================================  local data structures */


/* ================================================  local constants & macros */

/*------------------------------------------------------------------------------
 *  File identity
 *----------------------------------------------------------------------------*/
static const char fileid[] = "$Id$";


/* ===============================================  local function prototypes */


/* =============================================================  module code */

/*------------------------------------------------------------------------------
 *  Calculate the length of a zero-terminated C string,
 *  w/o the zero-termination
 *----------------------------------------------------------------------------*/
unsigned int
Util :: strLen( const char    * str )                   throw ( Exception )
{
    size_t      len;

    if ( !str ) {
        throw Exception( __FILE__, __LINE__, "no str");
    }

    len = strlen( str);

    return len;
}


/*------------------------------------------------------------------------------
 *  Copy the contents of a string into another
 *----------------------------------------------------------------------------*/
void
Util :: strCpy (    char          * dest,
                    const char    * src )               throw ( Exception )
{
    if ( !dest || !src ) {
        throw Exception( __FILE__, __LINE__, "no src or dest");
    }

    strcpy( dest, src);
}


/*------------------------------------------------------------------------------
 *  Concatenate the contents of a string onto another
 *----------------------------------------------------------------------------*/
void
Util :: strCat (    char          * dest,
                    const char    * src )               throw ( Exception )
{
    if ( !dest || !src ) {
        throw Exception( __FILE__, __LINE__, "no src or dest");
    }

    strcat( dest, src);
}


/*------------------------------------------------------------------------------
 *  Duplicate a string by allocating space with new[]
 *  The returned string must be freed with delete[]
 *----------------------------------------------------------------------------*/
char *
Util :: strDup( const char    * str )                   throw ( Exception )
{
    size_t      len;
    char      * s;

    if ( !str ) {
        throw Exception( __FILE__, __LINE__, "no str");
    }

    len = strlen( str) + 1;
    s   = new char[len];
    memcpy( s, str, len);

    return s;
}


/*------------------------------------------------------------------------------
 *  Check wether two strings are equal
 *----------------------------------------------------------------------------*/
bool
Util :: strEq( const char    * str1,
               const char    * str2 )                   throw ( Exception )
{
    if ( !str1 || !str2 ) {
        throw Exception( __FILE__, __LINE__, "no str1 or no str2");
    }

    return !strcmp( str1, str2);
}


/*------------------------------------------------------------------------------
 *  Convert a string to a long integer
 *----------------------------------------------------------------------------*/
long int
Util :: strToL( const char    * str,
                int             base )                  throw ( Exception )
{
    long int    val;
    char      * s;

    if ( !str ) {
        throw Exception( __FILE__, __LINE__, "no str");
    }

    val = strtol( str, &s, base);
    if ( s == str || val == LONG_MIN || val == LONG_MAX ) {
        throw Exception( __FILE__, __LINE__, "number conversion error");
    }

    return val;
}



/*------------------------------------------------------------------------------
 
  $Source$

  $Log$
  Revision 1.6  2001/08/30 17:25:56  darkeye
  renamed configure.h to config.h

  Revision 1.5  2000/11/12 13:31:40  darkeye
  added kdoc-style documentation comments

  Revision 1.4  2000/11/09 22:04:33  darkeye
  added functions strLen strCpy and strCat

  Revision 1.3  2000/11/09 06:44:21  darkeye
  added strEq and strToL functions

  Revision 1.2  2000/11/05 14:08:28  darkeye
  changed builting to an automake / autoconf environment

  Revision 1.1.1.1  2000/11/05 10:05:55  darkeye
  initial version

  
------------------------------------------------------------------------------*/

