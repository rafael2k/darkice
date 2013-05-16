/*------------------------------------------------------------------------------

   Copyright (c) 2000-2007 Tyrell Corporation. All rights reserved.

   Tyrell Config

   File     : DarkIceConfig.h
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
#ifndef CONFIG_H
#define CONFIG_H

#ifndef __cplusplus
#error This is a C++ include file
#endif


/* ============================================================ include files */

#include <map>
#include <string>

#include <iostream>

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
 *  @author  $Author: rafael@riseup.net $
 *  @version $Revision: 474 $
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
        Config (    std::istream  & is )            throw ( Exception )
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
         *  @param is the input stream to read from
         *  @return true if the line was correct, false otherwise.
         *  @exception Exception
         */
        virtual void
        read (   std::istream     & is )                throw ( Exception );

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

