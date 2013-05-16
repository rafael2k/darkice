/*------------------------------------------------------------------------------

   Copyright (c) 2000-2007 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : Exception.cpp
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

/* ============================================================ include files */

#ifdef HAVE_CONFIG_H
#include "config.h"
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
static const char fileid[] = "$Id: Exception.cpp 474 2010-05-10 01:18:15Z rafael@riseup.net $";


/* ===============================================  local function prototypes */


/* =============================================================  module code */

/*------------------------------------------------------------------------------
 *  Constructor
 *----------------------------------------------------------------------------*/
Exception :: Exception (    const char            * file,
                            unsigned int            line,
                            const char            * description1,
                            const char            * description2,
                            int                     code )          throw ()
{
    size_t      len = 0;

    if ( description1 ) {
        len += strlen( description1);
    }
    if ( description2 ) {
        len += strlen( description2);
    }

    if ( len ) {
        char      * str = new char[len+1];

        str[0] = '\0';
        if ( description1 ) {
            strcat( str, description1);
        }
        if ( description2 ) {
            strcat( str, description2);
        }

        init( file, line, str, code);
        delete[] str;

    } else {

        init( file, line, 0, code);
    }
}


/*------------------------------------------------------------------------------
 *  Constructor
 *----------------------------------------------------------------------------*/
Exception :: Exception (    const char            * file,
                            unsigned int            line,
                            const char            * description1,
                            const char            * description2,
                            const char            * description3,
                            int                     code )          throw ()
{
    size_t      len = 0;

    if ( description1 ) {
        len += strlen( description1);
    }
    if ( description2 ) {
        len += strlen( description2);
    }
    if ( description3 ) {
        len += strlen( description3);
    }

    if ( len ) {
        char      * str = new char[len+1];

        str[0] = '\0';
        if ( description1 ) {
            strcat( str, description1);
        }
        if ( description2 ) {
            strcat( str, description2);
        }
        if ( description3 ) {
            strcat( str, description3);
        }

        init( file, line, str, code);
        delete[] str;

    } else {

        init( file, line, 0, code);
    }
}


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



