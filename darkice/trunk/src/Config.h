/*------------------------------------------------------------------------------

   Copyright (c) 2000 Tyrell Corporation. All rights reserved.

   Tyrell Config

   File     : Config.h
   Version  : $Revision$
   Author   : $Author$
   Location : $Source$
   
   Abstract : 

     A configuration file representation. The file is of the syntax:

     [section1]
     # this is a whole line comment
     key = value
     an ugly key name = long value    # this end is a comment too

     [section2]
     # this is a whole line comment in section 2
     key = value
     an ugly key name = long value    # this end is a comment too


     also empty lines are ignored and all white space is removed
     from the front and end of keys / values

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
#ifndef CONFIG_H
#define CONFIG_H

#ifndef __cplusplus
#error This is a C++ include file
#endif


/* ============================================================ include files */

#include <hash_map>
#include <string>

#include <iostream.h>

#include "Referable.h"
#include "ConfigSection.h"


/* ================================================================ constants */


/* =================================================================== macros */


/* =============================================================== data types */

/*------------------------------------------------------------------------------
 *  
 *----------------------------------------------------------------------------*/
class Config : public virtual Referable
{
    private:

        typedef hash_map<string, ConfigSection>     TableType;
        TableType                                   table;


        string                                      currentSection;


    protected:


    public:

        inline
        Config ( void )                             throw ( Exception )
        {
        }


        inline
        Config (    istream   & is )                throw ( Exception )
        {
            read( is );
        }


        inline virtual
        ~Config ( void )                            throw ( Exception )
        {
        }


/*

        inline
        Config ( const Config   & di )            throw ( Exception )
        {
        }


        inline Config &
        operator= ( const Config   * di )          throw ( Exception )
        {
        }
*/

        inline virtual void
        reset ( void )                                  throw ( Exception )
        {
            table.clear();
            currentSection = "";
        }


        virtual bool
        addLine (   const char    * line )              throw ( Exception );


        virtual void
        read (   istream &    is )                      throw ( Exception );


        virtual const ConfigSection *
        get (   const char    * key ) const             throw ( Exception );
};


/* ================================================= external data structures */


/* ====================================================== function prototypes */



#endif  /* CONFIG_H */


/*------------------------------------------------------------------------------
 
  $Source$

  $Log$
  Revision 1.2  2000/11/09 22:07:19  darkeye
  added constructor with istream

  Revision 1.1  2000/11/08 17:29:50  darkeye
  added configuration file reader

  Revision 1.1.1.1  2000/11/05 10:05:50  darkeye
  initial version

  
------------------------------------------------------------------------------*/

