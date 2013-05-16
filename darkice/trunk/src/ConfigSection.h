/*------------------------------------------------------------------------------

   Copyright (c) 2000-2007 Tyrell Corporation. All rights reserved.

   Tyrell ConfigSection

   File     : ConfigSection.h
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
#ifndef CONFIG_SECTION_H
#define CONFIG_SECTION_H

#ifndef __cplusplus
#error This is a C++ include file
#endif


/* ============================================================ include files */

#include <map>
#include <string>

#include "Referable.h"


/* ================================================================ constants */


/* =================================================================== macros */


/* =============================================================== data types */

/**
 *  A configuration file representation. The file is of the syntax:
 *
 *  <pre>
 *  # this is a whole line comment
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
class ConfigSection : public virtual Referable
{
    private:

        /**
         *  Type of the hash table used in this class.
         */
        typedef std::map<std::string, std::string>      TableType;

        /**
         *  Hash table holding the configuration information.
         */
        TableType                               table;


    protected:


    public:

        /**
         *  Default constructor.
         *
         *  @exception Exception
         */
        inline
        ConfigSection ( void )                            throw ( Exception )
        {
        }

        /**
         *  Destructor.
         *
         *  @exception Exception
         */
        inline virtual
        ~ConfigSection ( void )                           throw ( Exception )
        {
        }


/* TODO

        inline
        ConfigSection ( const ConfigSection   & di )            throw ( Exception )
        {
        }


        inline ConfigSection &
        operator= ( const ConfigSection   * di )          throw ( Exception )
        {
        }
*/

        /**
         *  Add a key / value pair to the configuration information.
         *
         *  @param key the key to add the value by
         *  @param value the value to add for the key
         *  @return true if adding was successful, false otherwise
         *  @exception Exception
         */
        virtual bool
        add (   const char    * key,
                const char    * value )                 throw ( Exception );

        /**
         *  Get a value for a key.
         *
         *  @param key the key to get the value for
         *  @return the value for the key, or NULL if the key doesn't exist.
         *  @exception Exception
         */
        virtual const char *
        get (   const char    * key ) const             throw ( Exception );

        /**
         *  Get a value for a key, or throw an Exception.
         *
         *  @param key the key to get the value for
         *  @param message1 message part 1 of the Exception to be thrown.
         *  @param message2 message part 2 of the Exception to be thrown.
         *  @param code error code of the Exception to be thrown.
         *  @return the value for the key. The return value is never NULL.
         *  @exception Exception
         */
        virtual const char *
        getForSure (    const char    * key,
                        const char    * message1 = 0,
                        const char    * message2 = 0,
                        int             code     = 0 ) const
                                                        throw ( Exception );

        /**
         *  Add a line of configuration information.
         *
         *  @param line the line to add.
         *  @return true if a new key was added, false otherwise.
         *  @exception Exception
         */
        virtual bool
        addLine (   const char    * line )              throw ( Exception );
};


/* ================================================= external data structures */


/* ====================================================== function prototypes */



#endif  /* CONFIG_SECTION_H */

