/*------------------------------------------------------------------------------

   Copyright (c) 2000 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : FileSink.cpp
   Version  : $Revision$
   Author   : $Author$
   Location : $Source$
   
   Abstract : 

     File data output

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

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#else
#error need unistd.h
#endif

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#else
#error need stdlib.h
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

#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#else
#error need sys/time.h
#endif


#include "Util.h"
#include "Exception.h"
#include "FileSink.h"


/* ===================================================  local data structures */


/* ================================================  local constants & macros */

/*------------------------------------------------------------------------------
 *  File identity
 *----------------------------------------------------------------------------*/
static const char fileid[] = "$Id$";


/* ===============================================  local function prototypes */


/* =============================================================  module code */

/*------------------------------------------------------------------------------
 *  Initialize the object
 *----------------------------------------------------------------------------*/
void
FileSink :: init (  const char    * name )          throw ( Exception )
{
    fileName = Util::strDup( name);
    fileDescriptor = 0;
}


/*------------------------------------------------------------------------------
 *  De-initialize the object
 *----------------------------------------------------------------------------*/
void
FileSink :: strip ( void)                           throw ( Exception )
{
    if ( isOpen() ) {
        close();
    }

    delete[] fileName;
}


/*------------------------------------------------------------------------------
 *  Copy Constructor
 *----------------------------------------------------------------------------*/
FileSink :: FileSink (  const FileSink &    fs )    throw ( Exception )
                : Sink( fs )
{
    int     fd;
    
    init( fs.fileName);
    
    if ( (fd = fs.fileDescriptor ? dup( fs.fileDescriptor) : 0) == -1 ) {
        strip();
        throw Exception( __FILE__, __LINE__, "dup failure");
    }

    fileDescriptor = fd;
}


/*------------------------------------------------------------------------------
 *  Assignment operator
 *----------------------------------------------------------------------------*/
FileSink &
FileSink :: operator= (  const FileSink &    fs )   throw ( Exception )
{
    if ( this != &fs ) {
        int     fd;

        /* first strip */
        strip();


        /* then build up */
        Sink::operator=( fs );
        
        init( fs.fileName);
        
        if ( (fd = fs.fileDescriptor ? dup( fs.fileDescriptor) : 0) == -1 ) {
            strip();
            throw Exception( __FILE__, __LINE__, "dup failure");
        }

        fileDescriptor = fd;
    }

    return *this;
}


/*------------------------------------------------------------------------------
 *  Check wether a file exists
 *----------------------------------------------------------------------------*/
bool
FileSink :: exists ( void ) const               throw ()
{
    struct stat     st;

    if ( stat( (const char*)fileName, &st) == -1 ) {
        return false;
    }

    return true;
}


/*------------------------------------------------------------------------------
 *  Create a file, truncate if already exists
 *----------------------------------------------------------------------------*/
bool
FileSink :: create ( void )                     throw ( Exception )
{
    int     fd;
    
    if ( isOpen() ) {
        return false;
    }

    if ( (fd = ::creat( fileName, S_IRUSR | S_IWUSR)) == -1 ) {
        throw Exception( __FILE__, __LINE__, "creat error", errno);
    }

    ::close( fd);
    return true;
}


/*------------------------------------------------------------------------------
 *  Open the file
 *----------------------------------------------------------------------------*/
bool
FileSink :: open ( void )                       throw ( Exception )
{
    if ( isOpen() ) {
        return false;
    }

    if ( (fileDescriptor = ::open( fileName, O_WRONLY | O_TRUNC, 0)) == -1 ) {
        fileDescriptor = 0;
        return false;
    }

    return true;
}


/*------------------------------------------------------------------------------
 *  Check wether read() would return anything
 *----------------------------------------------------------------------------*/
bool
FileSink :: canWrite (     unsigned int    sec,
                           unsigned int    usec )   throw ( Exception )
{
    fd_set              fdset;
    struct timeval      tv;
    int                 ret;

    if ( !isOpen() ) {
        return false;
    }

    FD_ZERO( &fdset);
    FD_SET( fileDescriptor, &fdset);
    tv.tv_sec  = sec;
    tv.tv_usec = usec;

    ret = select( fileDescriptor + 1, NULL, &fdset, NULL, &tv);
    
    if ( ret == -1 ) {
        throw Exception( __FILE__, __LINE__, "select error");
    }

    return ret > 0;
}


/*------------------------------------------------------------------------------
 *  Read from the audio source
 *----------------------------------------------------------------------------*/
unsigned int
FileSink :: write (    const void    * buf,
                       unsigned int    len )        throw ( Exception )
{
    ssize_t     ret;

    if ( !isOpen() ) {
        return 0;
    }

    ret = ::write( fileDescriptor, buf, len);

    if ( ret == -1 ) {
        if ( errno == EAGAIN ) {
            ret = 0;
        } else {
            throw Exception( __FILE__, __LINE__, "write error", errno);
        }
    }

    return ret;
}


/*------------------------------------------------------------------------------
 *  Close the audio source
 *----------------------------------------------------------------------------*/
void
FileSink :: close ( void )                          throw ( Exception )
{
    if ( !isOpen() ) {
        return;
    }

    flush();
    ::close( fileDescriptor);
    fileDescriptor = 0;
}



/*------------------------------------------------------------------------------
 
  $Source$

  $Log$
  Revision 1.2  2000/11/05 14:08:27  darkeye
  changed builting to an automake / autoconf environment

  Revision 1.1.1.1  2000/11/05 10:05:51  darkeye
  initial version

  
------------------------------------------------------------------------------*/

