/*------------------------------------------------------------------------------

   Copyright (c) 2000 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : PipeSink.cpp
   Version  : $Revision$
   Author   : $Author$
   Location : $Source$
   
   Abstract : 

     Audio data input

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

#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#else
#error need sys/stat.h
#endif

#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#else
#error need fcntl.h
#endif

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#else
#error need sys/types.h
#endif

#ifdef HAVE_ERRNO_H
#include <errno.h>
#else
#error need errno.h
#endif


#include "Exception.h"
#include "PipeSink.h"


/* ===================================================  local data structures */


/* ================================================  local constants & macros */

/*------------------------------------------------------------------------------
 *  File identity
 *----------------------------------------------------------------------------*/
static const char fileid[] = "$Id$";


/* ===============================================  local function prototypes */


/* =============================================================  module code */

/*------------------------------------------------------------------------------
 *  Create a pipe
 *----------------------------------------------------------------------------*/
bool
PipeSink :: create ( void )                     throw ( Exception )
{
    if ( isOpen() ) {
        return false;
    }

    if ( mkfifo( getFileName(), S_IRUSR | S_IWUSR) == -1 ) {
        if ( errno = EEXIST ) {
            return true;
        }
        throw Exception( __FILE__, __LINE__, "mkfifo error", errno);
    }

    return true;
}


/*------------------------------------------------------------------------------
 *  Open the file
 *----------------------------------------------------------------------------*/
bool
PipeSink :: open ( void )                       throw ( Exception )
{
    if ( isOpen() ) {
        return false;
    }

    if ( (fileDescriptor = ::open( getFileName(),
                                   O_WRONLY | O_NONBLOCK)) == -1 ) {
        fileDescriptor = 0;
        return false;
    }

    return true;
}



/*------------------------------------------------------------------------------
 
  $Source$

  $Log$
  Revision 1.2  2000/11/05 14:08:28  darkeye
  changed builting to an automake / autoconf environment

  Revision 1.1.1.1  2000/11/05 10:05:53  darkeye
  initial version

  
------------------------------------------------------------------------------*/

