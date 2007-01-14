/*------------------------------------------------------------------------------

   Copyright (c) 2000 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : main.cpp
   Version  : $Revision$
   Author   : $Author$
   Location : $Source$
   
   Abstract : 

    Program entry point

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
static const char fileid[] = "$Id$";

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
         << " live audio streamer, http://darkice.sourceforge.net"
         << std::endl
         << "Copyright (c) 2000-2006, Tyrell Hungary, http://tyrell.hu"
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


/*------------------------------------------------------------------------------
 
  $Source$

  $Log$
  Revision 1.16  2006/01/25 22:47:15  darkeye
  added mpeg2 support, thanks to Nicholas J Humfrey

  Revision 1.15  2005/04/14 11:24:42  darkeye
  updated copyright notice to extend to 2005

  Revision 1.14  2005/04/03 05:10:07  jbebel
  Moved initialization of Reporter class so it would happen before
  instantiation of Darkice class.  Any logging that might be reported
  during the construction of the Darkice class could not function.
  Originally the Reporter initialization was done through the instance
  of Darkice (which inherits Reporter), but that obviously isn't possible
  before Darkice is instantiated.  Since Reporter is largely a static class,
  it is reasonable to call it via the scope resolution operator rather
  than via an instance of the class, so that's what I did.

  Revision 1.13  2004/02/15 12:06:30  darkeye
  added ALSA support, thanks to Christian Forster

  Revision 1.12  2003/02/09 15:09:41  darkeye
  for version 0.13

  Revision 1.11  2002/05/28 12:35:41  darkeye
  code cleanup: compiles under gcc-c++ 3.1, using -pedantic option

  Revision 1.10  2002/02/20 15:08:52  darkeye
  minor changes

  Revision 1.9  2001/09/11 15:05:21  darkeye
  added Solaris support

  Revision 1.8  2001/09/02 12:24:29  darkeye
  now displays usage info when no command line parameters given

  Revision 1.7  2001/08/30 17:25:56  darkeye
  renamed configure.h to config.h

  Revision 1.6  2001/08/26 08:43:13  darkeye
  added support for unlimited time encoding

  Revision 1.5  2000/11/15 18:08:43  darkeye
  added multiple verbosity-level event reporting and verbosity command
  line option

  Revision 1.4  2000/11/13 20:21:29  darkeye
  added program version display on startup

  Revision 1.3  2000/11/13 19:38:55  darkeye
  moved command line parameter parsing from DarkIce.cpp to main.cpp

  Revision 1.2  2000/11/08 17:29:50  darkeye
  added configuration file reader

  Revision 1.1.1.1  2000/11/05 10:05:52  darkeye
  initial version

  
------------------------------------------------------------------------------*/

