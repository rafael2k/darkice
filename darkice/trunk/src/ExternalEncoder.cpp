/*------------------------------------------------------------------------------

   Copyright (c) 2000 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : ExternalEncoder.cpp
   Version  : $Revision$
   Author   : $Author$
   Location : $Source$
   
   Abstract : 

     A class representing an external audio encoder which is invoked
     with a frok() and an exec() call

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

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#else
#error need sys/types.h
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#else
#error need unistd.h
#endif

#ifdef HAVE_SIGNAL_H
#include <signal.h>
#else
#error need signal.h
#endif

#ifdef HAVE_ERRNO_H
#include <errno.h>
#else
#error need errno.h
#endif


#include "Exception.h"
#include "Util.h"
#include "ExternalEncoder.h"


/* ===================================================  local data structures */


/* ================================================  local constants & macros */

/*------------------------------------------------------------------------------
 *  File identity
 *----------------------------------------------------------------------------*/
static const char fileid[] = "$Id$";


/*------------------------------------------------------------------------------
 *  File identity
 *----------------------------------------------------------------------------*/
#define ARG_LEN     64


/* ===============================================  local function prototypes */


/* =============================================================  module code */

/*------------------------------------------------------------------------------
 *  Initialize the class
 *----------------------------------------------------------------------------*/
void
ExternalEncoder :: init (   const char    * encoderName,
                            const char    * inFileName,
                            const char    * outFileName )   throw ( Exception )
{
    unsigned int    u;

    for ( u = 0; u < numCmdArgs; ++u ) {
        cmdArgs[u] = 0;
    }

    this->encoderName = Util::strDup( encoderName);
    this->inFileName  = Util::strDup( inFileName);
    this->outFileName = Util::strDup( outFileName);
    this->child       = 0;
}


/*------------------------------------------------------------------------------
 *  De-initialize the class
 *----------------------------------------------------------------------------*/
void
ExternalEncoder :: strip ( void )                   throw ( Exception )
{
    unsigned int    u;

    if ( isRunning() ) {
        stop();
    }

    for ( u = 0; u < numCmdArgs; ++u ) {
        if ( cmdArgs[u] ) {
            delete[] cmdArgs[u];
        }
    }

    delete[] encoderName;
    delete[] inFileName;
    delete[] outFileName;
    child = 0;
}


/*------------------------------------------------------------------------------
 *  Set the nth command line argument
 *----------------------------------------------------------------------------*/
void
ExternalEncoder :: setArg ( const char    * str,
                            unsigned int    index )     throw ( Exception )
{
    if ( index >= numCmdArgs ) {
        throw Exception( __FILE__, __LINE__, "index >= numCmdArgs", index);
    }

    cmdArgs[index] = str ? Util::strDup( str) : 0;
}


/*------------------------------------------------------------------------------
 *  Start the encoding
 *----------------------------------------------------------------------------*/
bool
ExternalEncoder :: start ( void )           throw ( Exception )
{
    pid_t   pid;

    if ( isRunning() ) {
        return false;
    }

    pid = fork();

    if ( pid == -1 ) {
        throw Exception( __FILE__, __LINE__, "fork error");
    } else if ( pid == 0 ) {
        cout << "wow, I'm a voodoo child!" << endl;

        makeArgs();
        execvp( getEncoderName(), cmdArgs);

        throw Exception( __FILE__, __LINE__, "exec returned");
    } else {
        child = pid;
        cout << "I'm a parent, the child's pid is " << child << endl;

        return true;
    }
}


/*------------------------------------------------------------------------------
 *  End the encoding
 *----------------------------------------------------------------------------*/
void
ExternalEncoder :: stop ( void )            throw ( Exception )
{
    if ( !isRunning() ) {
        return;
    }

    if ( kill( child, SIGHUP) == -1 ) {
        throw Exception( __FILE__, __LINE__, "kill", errno);
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

