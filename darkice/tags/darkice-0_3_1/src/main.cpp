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
#include "configure.h"
#endif

#ifdef HAVE_GETOPT_H
#include <getopt.h>
#else
#error need getopt.h
#endif

#include <iostream.h>
#include <fstream.h>

#include "Ref.h"
#include "Exception.h"
#include "Util.h"
#include "DarkIce.h"


/* ===================================================  local data structures */


/* ================================================  local constants & macros */

/*------------------------------------------------------------------------------
 *  File identity
 *----------------------------------------------------------------------------*/
static const char fileid[] = "$Id$";


/* ===============================================  local function prototypes */

/*------------------------------------------------------------------------------
 *  Show program usage
 *----------------------------------------------------------------------------*/
static void
showUsage (     ostream   & os );


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

    cout << "DarkIce " << VERSION
         << " live audio streamer, http://darkice.sourceforge.net" << endl;
    cout << "Copyright (c) 2000-2001, Tyrell Hungary, http://tyrell.hu" << endl;
    cout << endl;

    try {
        const char    * configFileName = 0;
        unsigned int    verbosity      = 1;
        int             i;
        const char      opts[] = "hc:v:";
        static struct option long_options[] = {
            { "config", 1, 0, 'c'},
            { "help", 0, 0, 'h'},
            { "verbosity", 1, 0, 'v'},
            { 0, 0, 0, 0}
        };
            

        while ( (i = getopt_long( argc, argv, opts, long_options, 0)) != -1 ) {
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
                    showUsage( cout);
                    return 1;
            }
        }

        if ( !configFileName ) {
            throw Exception( __FILE__, __LINE__,
                             "no configuration file specified");
        }

        cout << "Using config file: " << configFileName << endl;

        ifstream            configFile( configFileName);
        Config              config( configFile);
        Ref<DarkIce>        di = new DarkIce( config);
        di->setReportVerbosity( verbosity );
        di->setReportOutputStream( cout );

        res = di->run();

    } catch ( Exception   & e ) {
        cout << "DarkIce: " << e << endl << flush;
    }

    return res;
}


/*------------------------------------------------------------------------------
 *  Show program usage
 *----------------------------------------------------------------------------*/
static void
showUsage (     ostream   & os )
{
    os
    << "usage: darkice -c config.file"
    << endl
    << endl
    << "options:"
    << endl
    << "   -c, --config=config.file    use configuration file config.file"
    << endl
    << "   -v, --verbosity=number      verbosity level (0 = silent, 10 = loud)"
    << endl
    << "   -h, --help                  print this message and exit"
    << endl
    << endl;
}


/*------------------------------------------------------------------------------
 
  $Source$

  $Log$
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

