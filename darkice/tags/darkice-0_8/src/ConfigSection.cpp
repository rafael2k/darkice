/*------------------------------------------------------------------------------

   Copyright (c) 2000 Tyrell Corporation. All rights reserved.

   Tyrell ConfigSection

   File     : ConfigSection.cpp
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

#include <iterator>

#include <iostream.h>


#include "ConfigSection.h"


/* ===================================================  local data structures */


/* ================================================  local constants & macros */

/*------------------------------------------------------------------------------
 *  File identity
 *----------------------------------------------------------------------------*/
static const char fileid[] = "$Id$";


/*------------------------------------------------------------------------------
 *  string containing all white space characters
 *----------------------------------------------------------------------------*/
#define WHITE_SPACE_STR      " \t"


/* ===============================================  local function prototypes */


/* =============================================================  module code */

/*------------------------------------------------------------------------------
 *  Add a key / value pair
 *----------------------------------------------------------------------------*/
bool
ConfigSection :: add (   const char    * key,
                         const char    * value )            throw ( Exception )
{
    if ( !key || !value ) {
        throw Exception( __FILE__, __LINE__, "no key or value");
    }

    std::pair<const std::string, std::string>   element( key, value);
    std::pair<TableType::iterator, bool>        res;

    res = table.insert( element);

    return res.second;
}
        
        
/*------------------------------------------------------------------------------
 *  Get a value for a key
 *----------------------------------------------------------------------------*/
const char *
ConfigSection :: get (  const char    * key ) const         throw ( Exception )
{
    if ( !key ) {
        throw Exception( __FILE__, __LINE__, "no key");
    }

    TableType::const_iterator   it = table.find( key);
    if ( it == table.end() ) {
        return 0;
    }
    return it->second.c_str();
}


/*------------------------------------------------------------------------------
 *  Get a value for a key, in the key does not exist, throw an exception
 *----------------------------------------------------------------------------*/
const char *
ConfigSection :: getForSure (   const char    * key,
                                const char    * message1 = 0,
                                const char    * message2 = 0,
                                int             code     = 0 ) const
                                                        throw ( Exception )
{
    const char      * value;
    
    if ( !(value = get( key)) ) {
        throw Exception( __FILE__, __LINE__, key, message1, message2, code);
    }

    return value;
}


/*------------------------------------------------------------------------------
 *  Add a configuration line
 *----------------------------------------------------------------------------*/
bool
ConfigSection :: addLine (  const char    * line )          throw ( Exception )
{
    if ( !line ) {
        throw Exception( __FILE__, __LINE__, "no line");
    }

    std::string::size_type  ix;
    std::string             str( line);

    /* delete everything after the first # */
    if ( (ix = str.find( '#')) != str.npos ) {
        str.erase( ix);
    }
    /* eat up all white space from the front */
    if ( (ix = str.find_first_not_of( WHITE_SPACE_STR)) != str.npos ) {
        str.erase( 0, ix);
    }
    /* eat up all white space from the end */
    if ( (ix = str.find_last_not_of( WHITE_SPACE_STR)) != str.npos ) {
        str.erase( ix + 1);
    }
    if ( !str.length() ) {
        return true;
    }

    /* find the '=' delimiter between key and value */
    if ( (ix = str.find( '=')) == str.npos ) {
        return false;
    }

    std::string     key( str, 0, ix);
    std::string     value( str, ix + 1);

    /* eat up all white space from the front of value */
    if ( (ix = value.find_first_not_of( WHITE_SPACE_STR)) != value.npos ) {
        value.erase( 0, ix);
    }
    /* eat up all white space from the end of key */
    if ( (ix = key.find_last_not_of( WHITE_SPACE_STR)) != key.npos ) {
        key.erase( ix + 1);
    }

    /* now add the new key / value pair */
    return add( key.c_str(), value.c_str());
}


/*------------------------------------------------------------------------------
 
  $Source$

  $Log$
  Revision 1.7  2001/10/19 09:20:09  darkeye
  config file now may contain tabs also as white space

  Revision 1.6  2001/09/09 11:26:43  darkeye
  full line comments skipped earlier: commens allowed before the first secion

  Revision 1.5  2001/09/05 20:11:15  darkeye
  removed dependency on locally stored SGI STL header files
  now compiler-supplied C++ library STL header files are used
  compiles under GNU C++ 3
  hash_map (an SGI extension to STL) replaced with map
  std:: namespace prefix added to all STL class references

  Revision 1.4  2001/08/30 17:25:56  darkeye
  renamed configure.h to config.h

  Revision 1.3  2000/11/13 18:46:50  darkeye
  added kdoc-style documentation comments

  Revision 1.2  2000/11/09 22:08:17  darkeye
  added function getForSure

  Revision 1.1  2000/11/08 17:29:50  darkeye
  added configuration file reader

  Revision 1.2  2000/11/05 14:08:27  darkeye
  changed builting to an automake / autoconf environment

  Revision 1.1.1.1  2000/11/05 10:05:49  darkeye
  initial version

  
------------------------------------------------------------------------------*/

