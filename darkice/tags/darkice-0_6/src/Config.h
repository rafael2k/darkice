/*------------------------------------------------------------------------------

   Copyright (c) 2000 Tyrell Corporation. All rights reserved.

   Tyrell Config

   File     : Config.h
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
#ifndef CONFIG_H
#define CONFIG_H

#ifndef __cplusplus
#error This is a C++ include file
#endif


/* ============================================================ include files */

#include <map>
#include <string>

#include <iostream.h>

#include "Referable.h"
#include "ConfigSection.h"


/* ================================================================ constants */


/* =================================================================== macros */


/* =============================================================== data types */

/**
 *  A configuration file representation. The file is of the syntax:
 *
 *  <pre>
 *  [section1]
 *  # this is a whole line comment
 *  key = value
 *  an ugly key name = long value    # this end is a comment too
 *
 *  [section2]
 *  # this is a whole line comment in section 2
 *  key = value
 *  an ugly key name = long value    # this end is a comment too
 *  </pre>
 *
 *  also empty lines are ignored and all white space is removed
 *  from the front and end of keys / values
 *
 *  Knwon problem: you can't use '#' in any part of a key / value pair
 *
 *  @author  $Author$
 *  @version $Revision$
 */
class Config : public virtual Referable
{
    private:

        /**
         *  Type declaration of the hash table type.
         */
        typedef std::map<std::string, ConfigSection>    TableType;

        /**
         *  Hash table holding the configuration sections.
         *
         *  @see ConfigSection
         */
        TableType                                   table;

        /**
         *  Hash table holding the configuration sections.
         *
         *  @see ConfigSection
         */
        std::string                                 currentSection;


    protected:


    public:

        /**
         *  Default constructor.
         *
         *  @exception Exception
         */
        inline
        Config ( void )                             throw ( Exception )
        {
        }

        /**
         *  Constructor based on an input stream.
         *
         *  @param is configuration will be read from this input stream
         *            until end of stream is reached.
         *  @exception Exception
         */
        inline
        Config (    istream   & is )                throw ( Exception )
        {
            read( is );
        }

        /**
         *  Destructor.
         *
         *  @exception Exception
         */
        inline virtual
        ~Config ( void )                            throw ( Exception )
        {
        }


/*  TODO

        inline
        Config ( const Config   & di )            throw ( Exception )
        {
        }


        inline Config &
        operator= ( const Config   * di )          throw ( Exception )
        {
        }
*/

        /**
         *  Delete the configuration information stored in the object.
         *  Resets the object to a clean state.
         *
         *  @exception Exception
         */
        inline virtual void
        reset ( void )                                  throw ( Exception )
        {
            table.clear();
            currentSection = "";
        }

        /**
         *  Read a line of confiugration information.
         *
         *  @param line the line to read.
         *  @return true if the line was correct, false otherwise.
         *  @exception Exception
         */
        virtual bool
        addLine (   const char    * line )              throw ( Exception );

        /**
         *  Read a line of confiugration information.
         *
         *  @param line the line to read.
         *  @return true if the line was correct, false otherwise.
         *  @exception Exception
         */
        virtual void
        read (   istream &    is )                      throw ( Exception );

        /**
         *  Get a ConfigSection by name.
         *
         *  @param key the name of the ConfigSection
         *  @return the ConfigSection requested, or NULL if it doesn't exists.
         *  @exception Exception
         */
        virtual const ConfigSection *
        get (   const char    * key ) const             throw ( Exception );
};


/* ================================================= external data structures */


/* ====================================================== function prototypes */



#endif  /* CONFIG_H */


/*------------------------------------------------------------------------------
 
  $Source$

  $Log$
  Revision 1.4  2001/09/05 20:11:15  darkeye
  removed dependency on locally stored SGI STL header files
  now compiler-supplied C++ library STL header files are used
  compiles under GNU C++ 3
  hash_map (an SGI extension to STL) replaced with map
  std:: namespace prefix added to all STL class references

  Revision 1.3  2000/11/13 18:46:50  darkeye
  added kdoc-style documentation comments

  Revision 1.2  2000/11/09 22:07:19  darkeye
  added constructor with istream

  Revision 1.1  2000/11/08 17:29:50  darkeye
  added configuration file reader

  Revision 1.1.1.1  2000/11/05 10:05:50  darkeye
  initial version

  
------------------------------------------------------------------------------*/

