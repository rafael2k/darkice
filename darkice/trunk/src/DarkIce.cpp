/*------------------------------------------------------------------------------

   Copyright (c) 2000 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : DarkIce.cpp
   Version  : $Revision$
   Author   : $Author$
   Location : $Source$
   

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

#ifdef HAVE_SYS_WAIT_H
#include <sys/wait.h>
#else
#error need sys/wait.h
#endif

#ifdef HAVE_ERRNO_H
#include <errno.h>
#else
#error need errno.h
#endif


#include <hash_map>
#include <string>


#include "Util.h"
#include "DarkIce.h"


/* ===================================================  local data structures */


/* ================================================  local constants & macros */

/*------------------------------------------------------------------------------
 *  File identity
 *----------------------------------------------------------------------------*/
static const char fileid[] = "$Id$";


/*------------------------------------------------------------------------------
 *  Make sure wait-related stuff is what we expect
 *----------------------------------------------------------------------------*/
#ifndef WEXITSTATUS
# define WEXITSTATUS(stat_val)      ((unsigned)(stat_val) >> 8)
#endif
#ifndef WIFEXITED
# define WIFEXITED(stat_val)        (((stat_val) & 255) == 0)
#endif



/* ===============================================  local function prototypes */


/* =============================================================  module code */

/*------------------------------------------------------------------------------
 *  Initialize the object
 *----------------------------------------------------------------------------*/
void
DarkIce :: init ( const Config      & config )              throw ( Exception )
{
    unsigned int             bufferSecs;
    const ConfigSection    * cs;
    const char             * str;
    unsigned int             sampleRate;
    unsigned int             bitsPerSample;
    unsigned int             channel;
    const char             * device;
    unsigned int             u;

    // the [general] section
    if ( !(cs = config.get( "general")) ) {
        throw Exception( __FILE__, __LINE__, "no section [general] in config");
    }
    str = cs->getForSure( "duration", " missing in section [general]");
    duration = Util::strToL( str);
    str = cs->getForSure( "bufferSecs", " missing in section [general]");
    bufferSecs = Util::strToL( str);


    // the [input] section
    if ( !(cs = config.get( "input")) ) {
        throw Exception( __FILE__, __LINE__, "no section [general] in config");
    }
    
    str = cs->getForSure( "sampleRate", " missing in section [input]");
    sampleRate = Util::strToL( str);
    str = cs->getForSure( "bitsPerSample", " missing in section [input]");
    bitsPerSample = Util::strToL( str);
    str = cs->getForSure( "channel", " missing in section [input]");
    channel = Util::strToL( str);
    device = cs->getForSure( "device", " missing in section [input]");

    dsp             = new OssDspSource( device,
                                        sampleRate,
                                        bitsPerSample,
                                        channel );
    encConnector    = new Connector( dsp.get());


    // look for lame encoder output streams, sections [lame0], [lame1], etc.
    char    lame[]          = "lame ";
    size_t  lameLen         = Util::strLen( lame);
    char  * pipeOutExt      = ".out";
    size_t  pipeOutExtLen   = Util::strLen( pipeOutExt);
    char  * pipeInExt       = ".in";
    size_t  pipeInExtLen    = Util::strLen( pipeInExt);

    for ( u = 0; u < maxOutput; ++u ) {
        // ugly hack to change the section name to "lame0", "lame1", etc.
        lame[lameLen-1] = '0' + u;

        if ( !(cs = config.get( lame)) ) {
            break;
        }

        const char    * encoder         = 0;
        unsigned int    bitrate         = 0;
        const char    * server          = 0;
        unsigned int    port            = 0;
        const char    * password        = 0;
        const char    * mountPoint      = 0;
        const char    * remoteDumpFile  = 0;
        const char    * name            = 0;
        const char    * description     = 0;
        const char    * url             = 0;
        const char    * genre           = 0;
        bool            isPublic        = false;
        unsigned int    lowpass         = 0;
        unsigned int    highpass        = 0;

        encoder     = cs->getForSure( "encoder", " missing in section ", lame);
        str         = cs->getForSure( "bitrate", " missing in section ", lame);
        bitrate     = Util::strToL( str);
        server      = cs->getForSure( "server", " missing in section ", lame);
        str         = cs->getForSure( "port", " missing in section ", lame);
        port        = Util::strToL( str);
        password    = cs->getForSure( "password", " missing in section ", lame);
        mountPoint  = cs->getForSure( "mountPoint"," missing in section ",lame);
        remoteDumpFile = cs->get( "remoteDumpFile");
        name        = cs->getForSure( "name", " missing in section ", lame);
        description = cs->getForSure("description"," missing in section ",lame);
        url         = cs->getForSure( "url", " missing in section ", lame);
        genre       = cs->getForSure( "genre", " missing in section ", lame);
        str         = cs->getForSure( "public", " missing in section ", lame);
        isPublic    = Util::strEq( str, "yes") ? true : false;
        str         = cs->get( "lowpass");
        lowpass     = str ? Util::strToL( str) : 0;
        str         = cs->get( "highpass");
        highpass    = str ? Util::strToL( str) : 0;

        // generate the pipe names
        char  pipeOutName[lameLen + pipeOutExtLen + 1];
        char  pipeInName[lameLen + pipeInExtLen + 1];

        Util::strCpy( pipeOutName, lame);
        Util::strCat( pipeOutName, pipeOutExt);
        Util::strCpy( pipeInName, lame);
        Util::strCat( pipeInName, pipeInExt);

        // go on and create the things

        outputs[u].pid = 0;

        // the pipes
        outputs[u].encOutPipe      = new PipeSource( pipeOutName);
        outputs[u].encInPipe       = new PipeSink( pipeInName);
        
        if ( !outputs[u].encOutPipe->exists() ) {
            if ( !outputs[u].encOutPipe->create() ) {
                throw Exception( __FILE__, __LINE__,
                                 "can't create out pipe ",
                                 pipeOutName );
            }
        }

        if ( !outputs[u].encInPipe->exists() ) {
            if ( !outputs[u].encInPipe->create() ) {
                throw Exception( __FILE__, __LINE__,
                                 "can't create in pipe",
                                 pipeInName );
            }
        }

        // encoder related stuff
        unsigned int bs = bufferSecs *
                          (bitsPerSample / 8) * channel * sampleRate;
        outputs[u].encIn    = new BufferedSink( outputs[u].encInPipe.get(),
                                                bs,
                                                (bitsPerSample / 8) * channel );
        reportEvent( 6, "using buffer size", bs);

        encConnector->attach( outputs[u].encIn.get());
        outputs[u].encoder     = new LameEncoder( encoder,
                                        outputs[u].encInPipe->getFileName(),
                                        dsp.get(),
                                        outputs[u].encOutPipe->getFileName(),
                                        bitrate,
                                        sampleRate,
                                        channel,
                                        lowpass,
                                        highpass );


        // streaming related stuff
        outputs[u].socket          = new TcpSocket( server, port);
        outputs[u].ice             = new IceCast( outputs[u].socket.get(),
                                                  password,
                                                  mountPoint,
                                                  remoteDumpFile,
                                                  name,
                                                  description,
                                                  url,
                                                  genre,
                                                  bitrate,
                                                  isPublic );
        outputs[u].shoutConnector  = new Connector( outputs[u].encOutPipe.get(),
                                                    outputs[u].ice.get());
    }

    noOutputs = u;
}


/*------------------------------------------------------------------------------
 *  Run the encoder
 *----------------------------------------------------------------------------*/
bool
DarkIce :: encode ( void )                          throw ( Exception )
{
    unsigned int       len;
    unsigned int       u;
    unsigned long      bytes;

    for ( u = 0; u < noOutputs; ++u ) {
        outputs[u].encoder->start();
    }

    sleep( 1 );

    if ( !encConnector->open() ) {
        throw Exception( __FILE__, __LINE__, "can't open connector");
    }
    
    bytes = dsp->getSampleRate() *
            (dsp->getBitsPerSample() / 8UL) *
            dsp->getChannel() *
            duration;
                                                
    len = encConnector->transfer( bytes,
                                  4096,
                                  1,
                                  0 );

    cout << len << " bytes transfered" << endl;

    encConnector->close();

    for ( u = 0; u < noOutputs; ++u ) {
        outputs[u].encoder->stop();
    }

    return true;
}


/*------------------------------------------------------------------------------
 *  Run the encoder
 *----------------------------------------------------------------------------*/
bool
DarkIce :: shout ( unsigned int     ix )                throw ( Exception )
{
    unsigned int       len;
    unsigned long      bytes;

    if ( ix >= noOutputs ) {
        return false;
    }

    if ( !outputs[ix].shoutConnector->open() ) {
        throw Exception( __FILE__, __LINE__, "can't open connector");
    }
    
    bytes = outputs[ix].encoder->getOutBitrate() * (1024UL / 8UL) * duration;
    len = outputs[ix].shoutConnector->transfer ( bytes, 4096, 10, 0 );

    cout << len << " bytes transfered" << endl;

    outputs[ix].shoutConnector->close();

    return true;
}


/*------------------------------------------------------------------------------
 *  Run
 *----------------------------------------------------------------------------*/
int
DarkIce :: run ( void )                             throw ( Exception )
{
    unsigned int    u;
    
    for ( u = 0; u < noOutputs; ++u ) {
        outputs[u].pid = fork();

        if ( outputs[u].pid == -1 ) {
            throw Exception( __FILE__, __LINE__, "fork error", errno);
            
        } else if ( outputs[u].pid == 0 ) {
            // this is the child

            sleep ( 1 );

            cout << "shouting " << u << endl << flush;
            shout( u);
            cout << "shouting ends " << u << endl << flush;

            exit(0);
        }
    }

    // this is the parent

    cout << "encoding" << endl << flush;
    encode();
    cout << "encoding ends" << endl << flush;

    for ( u = 0; u < noOutputs; ++u ) {
        int     status;

        waitpid( outputs[u].pid, &status, 0);

        if ( !WIFEXITED(status) ) {
            throw Exception( __FILE__, __LINE__,
                             "child exited abnormally", WEXITSTATUS(status));
        }
    }

    return 0;
}


/*------------------------------------------------------------------------------
 
  $Source$

  $Log$
  Revision 1.10  2000/11/17 15:50:48  darkeye
  added -Wall flag to compiler and eleminated new warnings

  Revision 1.9  2000/11/15 18:37:37  darkeye
  changed the transferable number of bytes to unsigned long

  Revision 1.8  2000/11/15 18:08:43  darkeye
  added multiple verbosity-level event reporting and verbosity command
  line option

  Revision 1.7  2000/11/13 19:38:55  darkeye
  moved command line parameter parsing from DarkIce.cpp to main.cpp

  Revision 1.6  2000/11/13 18:46:50  darkeye
  added kdoc-style documentation comments

  Revision 1.5  2000/11/10 20:16:21  darkeye
  first real tests with multiple streaming

  Revision 1.4  2000/11/09 22:09:46  darkeye
  added multiple outputs
  added configuration reading
  added command line processing

  Revision 1.3  2000/11/08 17:29:50  darkeye
  added configuration file reader

  Revision 1.2  2000/11/05 14:08:27  darkeye
  changed builting to an automake / autoconf environment

  Revision 1.1.1.1  2000/11/05 10:05:49  darkeye
  initial version

  
------------------------------------------------------------------------------*/

