/*------------------------------------------------------------------------------

   Copyright (c) 2000 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : DarkIce.h
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
#ifndef DARK_ICE_H
#define DARK_ICE_H

#ifndef __cplusplus
#error This is a C++ include file
#endif


/* ============================================================ include files */

#ifdef HAVE_CONFIG_H
#include "configure.h"
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#else
#error need unistd.h
#endif

#include <iostream.h>

#include "Referable.h"
#include "Reporter.h"
#include "Exception.h"
#include "Ref.h"
#include "OssDspSource.h"
#include "PipeSink.h"
#include "BufferedSink.h"
#include "Connector.h"
#include "LameEncoder.h"
#include "PipeSource.h"
#include "TcpSocket.h"
#include "IceCast.h"
#include "Config.h"


/* ================================================================ constants */


/* =================================================================== macros */


/* =============================================================== data types */

/**
 *  Program main object.
 *
 *  @author  $Author$
 *  @version $Revision$
 */
class DarkIce : public virtual Referable, public virtual Reporter
{
    private:

        /**
         *  The maximum number of supported outputs.
         */
        static const unsigned int       maxOutput = 8;
        
        /**
         *  Type describing each output.
         */
        typedef struct {
            Ref<PipeSink>           encInPipe;
            Ref<BufferedSink>       encIn;
            Ref<LameEncoder>        encoder;
            Ref<PipeSource>         encOutPipe;
            Ref<TcpSocket>          socket;
            Ref<IceCast>            ice;
            Ref<Connector>          shoutConnector;
            pid_t                   pid;
        } Output;

        /**
         *  Duration of playing, in seconds.
         */
        unsigned int            duration;

        /**
         *  The dsp to record from.
         */
        Ref<OssDspSource>       dsp;

        /**
         *  The encoding Connector, connecting the dsp to the encoders.
         */
        Ref<Connector>          encConnector;

        /**
         *  The outputs.
         */
        Output                  outputs[maxOutput];

        /**
         *  Number of outputs.
         */
        unsigned int            noOutputs;

        /**
         *  Original scheduling policy
         */
        int                     origSchedPolicy;

        /**
         *  Original scheduling priority
         */
        int                     origSchedPriority;

        /**
         *  Initialize the object.
         *
         *  @param config the config Object to read initialization
         *                information from.
         *  @exception Exception
         */
        void
        init (  const Config   & config )            throw ( Exception );

        /**
         *  Set POSIX real-time scheduling for the encoding process,
         *  if user permissions enable it.
         *
         *  @exception Exception
         */
        void
        setRealTimeScheduling ( void )              throw ( Exception );

        /**
         *  Set the scheduling that was before setting real-time scheduling.
         *  This function must be called _only_ after setRealTimeScheduling.
         *
         *  @exception Exception
         */
        void
        setOriginalScheduling ( void )              throw ( Exception );

        /**
         *  Start encoding. Spawns all encoders, opens the dsp and
         *  starts sending data to the encoders.
         *
         *  @return if encoding was successful.
         *  @exception Exception
         */
        bool
        encode ( void )                             throw ( Exception );

        /**
         *  Start shouting. fork()-s a process for each output, reads
         *  the output of the encoders and sends them to an IceCast server.
         *
         *  @return if shouting was successful.
         *  @exception Exception
         */
        bool
        shout ( unsigned int )                      throw ( Exception );


    protected:

        /**
         *  Default constructor. Always throws an Exception.
         *
         *  @exception Exception
         */
        inline
        DarkIce ( void )                            throw ( Exception )
        {
            throw Exception( __FILE__, __LINE__);
        }


    public:

        /**
         *  Constructor based on a configuration object.
         *
         *  @param config the config Object to read initialization
         *                information from.
         *  @exception Exception
         */
        inline
        DarkIce (   const Config  & config )       throw ( Exception )
        {
            init( config);
        }

        /**
         *  Destructor.
         *
         *  @exception Exception
         */
        inline virtual
        ~DarkIce ( void )                           throw ( Exception )
        {
        }

/* TODO

        inline
        DarkIce ( const DarkIce   & di )            throw ( Exception )
        {
        }


        inline DarkIce &
        operator= ( const DarkIce   * di )          throw ( Exception )
        {
        }
*/

        /**
         *  Run the process of recording / encoding / sending to the servers.
         *
         *  @return 0 on success
         *  @exception Exception
         */
        virtual int
        run ( void )                                throw ( Exception );

};


/* ================================================= external data structures */


/* ====================================================== function prototypes */



#endif  /* DARK_ICE_H */


/*------------------------------------------------------------------------------
 
  $Source$

  $Log$
  Revision 1.7  2000/12/20 12:36:47  darkeye
  added POSIX real-time scheduling

  Revision 1.6  2000/11/15 18:08:43  darkeye
  added multiple verbosity-level event reporting and verbosity command
  line option

  Revision 1.5  2000/11/13 19:38:55  darkeye
  moved command line parameter parsing from DarkIce.cpp to main.cpp

  Revision 1.4  2000/11/13 18:46:50  darkeye
  added kdoc-style documentation comments

  Revision 1.3  2000/11/10 20:16:21  darkeye
  first real tests with multiple streaming

  Revision 1.2  2000/11/09 22:09:46  darkeye
  added multiple outputs
  added configuration reading
  added command line processing

  Revision 1.1.1.1  2000/11/05 10:05:50  darkeye
  initial version

  
------------------------------------------------------------------------------*/

