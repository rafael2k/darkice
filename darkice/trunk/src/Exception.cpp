/*------------------------------------------------------------------------------

   Copyright (c) 2000 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : Exception.cpp
   Version  : $Revision$
   Author   : $Author$
   Location : $Source$
   
   Abstract : 

     An exception class

     This class should not depend on any other class
     (note: Cloneable is an interface) and should not throw
     any exceptions itself

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

/* ============================================================ include files */

#ifdef HAVE_CONFIG_H
#include "configure.h"
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#else
#error need string.h
#endif


#include "Exception.h"


/* ===================================================  local data structures */


/* ================================================  local constants & macros */

/*------------------------------------------------------------------------------
 *  File identity
 *----------------------------------------------------------------------------*/
static const char fileid[] = "$Id$";


/* ===============================================  local function prototypes */


/* =============================================================  module code */

/*------------------------------------------------------------------------------
 *  Initialize the class
 *----------------------------------------------------------------------------*/
void
Exception :: init ( const char            * file,
                    unsigned int            line,
                    const char            * description  = 0,
                    int                     code         = 0 )  throw ()
{
    if ( !file ) {
        this->file = 0;
    } else {
        size_t  len;
        
        len        = strlen( file ) + 1;
        this->file = new char[len];
        if ( this->file ) {
            memcpy( this->file, file, len);
        }
    }

    if ( !description ) {
        this->description = 0;
    } else {
        size_t  len;
        
        len               = strlen( description ) + 1;
        this->description = new char[len];
        if ( this->description ) {
            memcpy( this->description, description, len);
        }
    }

    this->line = line;
    this->code = code;
}


/*------------------------------------------------------------------------------
 *  De-initialize the class
 *----------------------------------------------------------------------------*/
void
Exception :: strip ( void )                         throw ()
{
    if ( description ) {
        delete[] description;
    }

    if ( file ) {
        delete[] file;
    }
}




/*------------------------------------------------------------------------------
 
  $Source$

  $Log$
  Revision 1.2  2000/11/05 14:08:27  darkeye
  changed builting to an automake / autoconf environment

  Revision 1.1.1.1  2000/11/05 10:05:50  darkeye
  initial version

  
------------------------------------------------------------------------------*/

