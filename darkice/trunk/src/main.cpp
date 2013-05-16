/*------------------------------------------------------------------------------

   Copyright (c) 2000-2007 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : main.cpp
   Version  : $Revision: 476 $
   Author   : $Author: rafael@riseup.net $
   Location : $HeadURL$
   
   Abstract : 

    Program entry point

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

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#else
#error needs stdlib.h
#endif

#ifdef HAVE_SIGNAL_H
#include <signal.h>
#else
#error needs signal.h
#endif

#include <iostream>
#include <fstream>

#include "Ref.h"
#include "Exception.h"
#include "Util.h"
#include "DarkIce.h"


/* ===================================================  local data structures */

/*------------------------------------------------------------------------------
 *  The DarkIce object we're running
 *----------------------------------------------------------------------------*/
static Ref<DarkIce>     darkice;


/* ================================================  local constants & macros */

/*------------------------------------------------------------------------------
 *  File identity
 *----------------------------------------------------------------------------*/
static const char fileid[] = "$Id: main.cpp 476 2010-05-10 01:30:13Z rafael@riseup.net $";

/*------------------------------------------------------------------------------
 *  Default config file name
 *----------------------------------------------------------------------------*/
static const char *DEFAULT_CONFIG_FILE = "/etc/darkice.cfg";


/* ===============================================  local function prototypes */

/*------------------------------------------------------------------------------
 *  Show program usage
 *----------------------------------------------------------------------------*/
static void
showUsage (     std::ostream  & os );

/*------------------------------------------------------------------------------
 *  Handler for the SIGUSR1 signal
 *----------------------------------------------------------------------------*/
static void
sigusr1Handler(int  value);


/* =============================================================  module code */

/*------------------------------------------------------------------------------
 *  Program entry point
 *----------------------------------------------------------------------------*/
int
main (
    int     argc,
    char  * argv[] )
{
    int     res = -1;

    std::cout << "DarkIce " << VERSION
         << " live audio streamer, http://code.google.com/p/darkice/"
         << std::endl
         << "Copyright (c) 2000-2007, Tyrell Hungary, http://tyrell.hu/"
         << std::endl
         << "Copyright (c) 2008-2010, Akos Maroy and Rafael Diniz" 
         << std::endl
         << "This is free software, and you are welcome to redistribute it "
         << std::endl
	 << "under the terms of The GNU General Public License version 3 or"
         << std::endl
	 << "any later version."
         << std::endl << std::endl;

    try {
        const char    * configFileName = DEFAULT_CONFIG_FILE;
        unsigned int    verbosity      = 1;
        int             i;
        const char      opts[] = "hc:v:";

        while ( (i = getopt( argc, argv, opts)) != -1 ) {
            switch ( i ) {
                case 'c':
                    configFileName = optarg;
                    break;

                case 'v':
                    verbosity = Util::strToL( optarg);
                    break;

                default:
                case ':':
                case '?':
                case 'h':
                    showUsage( std::cout);
                    return 1;
            }
        }

        std::cout << "Using config file: " << configFileName << std::endl;

        std::ifstream       configFile( configFileName);
        Reporter::setReportVerbosity( verbosity );
        Reporter::setReportOutputStream( std::cout );
        Config              config( configFile);

        darkice = new DarkIce( config);

        signal(SIGUSR1, sigusr1Handler);

        res = darkice->run();

    } catch ( Exception   & e ) {
        std::cout << "DarkIce: " << e << std::endl << std::flush;
    }

    return res;
}


/*------------------------------------------------------------------------------
 *  Show program usage
 *----------------------------------------------------------------------------*/
static void
showUsage (     std::ostream      & os )
{
    os
    << "usage: darkice [-v n] -c config.file"
    << std::endl
    << std::endl
    << "options:"
    << std::endl
    << "   -c config.file     use configuration file config.file"
    << std::endl
    << "                      if not specified, /etc/darkice.cfg is used"
    << std::endl
    << "   -v n               verbosity level (0 = silent, 10 = loud)"
    << std::endl
    << "   -h                 print this message and exit"
    << std::endl
    << std::endl;
}


/*------------------------------------------------------------------------------
 *  Handle the SIGUSR1 signal here
 *----------------------------------------------------------------------------*/
static void
sigusr1Handler(int    value)
{
    darkice->cut();
}

