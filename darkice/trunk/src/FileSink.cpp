/*------------------------------------------------------------------------------

   Copyright (c) 2000-2007 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : FileSink.cpp
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

#ifdef HAVE_STRING_H
#include <string.h>
#else
#error need string.h
#endif

#ifdef HAVE_SIGNAL_H
#include <signal.h>
#else
#error need signal.h
#endif


#include <iostream>
#include <sstream>
#include <fstream>


#include "Util.h"
#include "Exception.h"
#include "FileSink.h"


/* ===================================================  local data structures */


/* ================================================  local constants & macros */

/*------------------------------------------------------------------------------
 *  File identity
 *----------------------------------------------------------------------------*/
static const char fileid[] = "$Id: FileSink.cpp 474 2010-05-10 01:18:15Z rafael@riseup.net $";


/* ===============================================  local function prototypes */


/* =============================================================  module code */

/*------------------------------------------------------------------------------
 *  Initialize the object
 *----------------------------------------------------------------------------*/
void
FileSink :: init (  const char    * configName,
                    const char    * name )          throw ( Exception )
{
    this->configName  = Util::strDup(configName);
    fileName          = Util::strDup(name);
    fileDescriptor    = 0;
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
    
    init( fs.configName, fs.fileName);
    
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
        
        init( fs.configName, fs.fileName);
        
        if ( (fd = fs.fileDescriptor ? dup( fs.fileDescriptor) : 0) == -1 ) {
            strip();
            throw Exception( __FILE__, __LINE__, "dup failure");
        }

        fileDescriptor = fd;
    }

    return *this;
}


/*------------------------------------------------------------------------------
 *  Check wether a file exists and is regular file
 *----------------------------------------------------------------------------*/
bool
FileSink :: exists ( void ) const               throw ()
{
    struct stat     st;

    if ( stat( (const char*)fileName, &st) == -1 ) {
        return false;
    }

    return S_ISREG( st.st_mode);
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
        reportEvent( 3, "can't create file", fileName, errno);
        return false;
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
 *  Check wether the file can be written to
 *----------------------------------------------------------------------------*/
bool
FileSink :: canWrite (     unsigned int    sec,
                           unsigned int    usec )   throw ( Exception )
{
    fd_set              fdset;
    struct timespec     timespec;
    sigset_t            sigset;
    int                 ret;

    if ( !isOpen() ) {
        return false;
    }

    FD_ZERO( &fdset);
    FD_SET( fileDescriptor, &fdset);

    timespec.tv_sec  = sec;
    timespec.tv_nsec = usec * 1000L;

    // mask out SIGUSR1, as we're expecting that signal for other reasons
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGUSR1);

    ret = pselect( fileDescriptor + 1, NULL, &fdset, NULL, &timespec, &sigset);
    
    if ( ret == -1 ) {
        throw Exception( __FILE__, __LINE__, "select error");
    }

    return ret > 0;
}


/*------------------------------------------------------------------------------
 *  Write to the FileSink
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
 *  Get the file name to where to move the data saved so far.
 *  The trick is to read the file name from a file named
 *  /tmp/darkice.$configName.$PID , where:
 *   - $configName is the name of the configuration section for this file sink
 *   - $PID is the current process id
 *----------------------------------------------------------------------------*/
std::string
FileSink :: getArchiveFileName ( void )             throw ( Exception )
{
    pid_t               pid = getpid();
    std::stringstream   metaFileName;

    metaFileName << "/tmp/darkice." << configName << "." << pid;

    std::ifstream   ifs(metaFileName.str().c_str());
    if (!ifs.good()) {
        throw Exception(__FILE__, __LINE__, 
                        "can't find file ", metaFileName.str().c_str(), 0);
    }

    std::string     archiveFileName;
    ifs >> archiveFileName;
    ifs.close();

    return archiveFileName;
}

/*------------------------------------------------------------------------------
 *  Cut what we've done so far, and start anew.
 *----------------------------------------------------------------------------*/
void
FileSink :: cut ( void )                            throw ()
{
    flush();
    close();

    try {
        std::string     archiveFileName = getArchiveFileName();

        if (::rename(fileName, archiveFileName.c_str()) != 0) {
            reportEvent(2, "couldn't move file", fileName,
                           "to", archiveFileName);
        }

    } catch ( Exception &e ) {
        reportEvent(2, "error during archive cut", e);
    }

    create();
    open();
}


/*------------------------------------------------------------------------------
 *  Close the FileSink
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


