/*------------------------------------------------------------------------------

   Copyright (c) 2000 Tyrell Corporation. All rights reserved.

   Tyrell Config

   File     : Config.cpp
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
#include "configure.h"
#endif

#include <iterator>

#include <iostream.h>


#include "Config.h"


/* ===================================================  local data structures */


/* ================================================  local constants & macros */

/*------------------------------------------------------------------------------
 *  File identity
 *----------------------------------------------------------------------------*/
static const char fileid[] = "$Id$";


/*------------------------------------------------------------------------------
 *  Max line size
 *----------------------------------------------------------------------------*/
#define LINE_SIZE       256


/* ===============================================  local function prototypes */


/* =============================================================  module code */

/*------------------------------------------------------------------------------
 *  Get a value for a key
 *----------------------------------------------------------------------------*/
const ConfigSection *
Config :: get (  const char    * key ) const            throw ( Exception )
{
    if ( !key ) {
        throw Exception( __FILE__, __LINE__, "no key");
    }

    TableType::const_iterator   it = table.find( key);
    if ( it == table.end() ) {
        return 0;
    }
    return &(it->second);
}


/*------------------------------------------------------------------------------
 *  Add a configuration line
 *----------------------------------------------------------------------------*/
bool
Config :: addLine (  const char    * line )          throw ( Exception )
{
    if ( !line ) {
        throw Exception( __FILE__, __LINE__, "no line");
    }

    string::size_type   ix;
    string              str( line);

    /* delete everything after the first # */
    if ( (ix = str.find( '#')) != str.npos ) {
        str.erase( ix);
    }
    /* eat up all white space from the front */
    if ( (ix = str.find_first_not_of( ' ')) != str.npos ) {
        str.erase( 0, ix);
    }
    /* eat up all white space from the end */
    if ( (ix = str.find_last_not_of( ' ')) != str.npos ) {
        str.erase( ix + 1);
    }

    if ( str[0] == '[' && str[str.size()-1] == ']' ) {
        // a new section starts

        string                              section( str, 1, str.size()-2);
        ConfigSection                       cSection;
        pair<const string, ConfigSection>   element( section, cSection);
        pair<TableType::iterator, bool>     res;

        res = table.insert( element);

        currentSection = section;
        return res.second;
    } else {
        // it's a line for the current section

        TableType::iterator     it = table.find( currentSection);
        if ( it == table.end() ) {
            throw Exception( __FILE__, __LINE__, "no current section");
        }

        return it->second.addLine( line);
    }
}


/*------------------------------------------------------------------------------
 *  Add a configuration line
 *----------------------------------------------------------------------------*/
void
Config :: read (    istream &     is  )                 throw ( Exception )
{
    char            line[LINE_SIZE];
    unsigned int    num;

    for ( num = 0; !is.fail() && !is.eof(); ++num ) {
        is.getline( line, LINE_SIZE);
        if ( is.eof() ) {
            break;
        } else if ( is.fail() ) {
            throw Exception( __FILE__, __LINE__, "line too long", num);
        }

        addLine( line);
    }
}

    

/*------------------------------------------------------------------------------
 
  $Source$

  $Log$
  Revision 1.2  2000/11/13 18:46:50  darkeye
  added kdoc-style documentation comments

  Revision 1.1  2000/11/08 17:29:50  darkeye
  added configuration file reader

  Revision 1.2  2000/11/05 14:08:27  darkeye
  changed builting to an automake / autoconf environment

  Revision 1.1.1.1  2000/11/05 10:05:49  darkeye
  initial version

  
------------------------------------------------------------------------------*/

