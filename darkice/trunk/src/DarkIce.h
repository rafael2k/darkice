/*------------------------------------------------------------------------------

   Copyright (c) 2000-2007 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : DarkIce.h
   Version  : $Revision$
   Author   : $Author$
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
#ifndef DARK_ICE_H
#define DARK_ICE_H

#ifndef __cplusplus
#error This is a C++ include file
#endif


/* ============================================================ include files */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#else
#error need unistd.h
#endif

#include <iostream>

#include "Referable.h"
#include "Reporter.h"
#include "Exception.h"
#include "Ref.h"
#include "AudioSource.h"
#include "BufferedSink.h"
#include "Connector.h"
#include "AudioEncoder.h"
#include "TcpSocket.h"
#include "CastSink.h"
#include "DarkIceConfig.h"


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
         *  The maximum number of supported outputs. This should be
         *  <supported output types> * <outputs per type>
         */
        static const unsigned int       maxOutput = 4 * 7;
        
        /**
         *  Type describing each lame library output.
         */
        typedef struct {
            Ref<Sink>               encoder;
            Ref<TcpSocket>          socket;
            Ref<CastSink>           server;
        } Output;

        /**
         *  The outputs.
         */
        Output                  audioOuts[maxOutput];

        /**
         *  Number of lame library outputs.
         */
        unsigned int            noAudioOuts;

        /**
         *  Duration of playing, in seconds.
         */
        unsigned int            duration;

        /**
         *  The dsp to record from.
         */
        Ref<AudioSource>        dsp;

        /**
         *  The encoding Connector, connecting the dsp to the encoders.
         */
        Ref<Connector>          encConnector;

        /**
         *  Should we turn real-time scheduling on ?
         */
        int                     enableRealTime;

        /**
         *  Scheduling priority for the realtime threads
         */
        int                     realTimeSchedPriority;

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
         *  Look for the icecast stream outputs from the config file.
         *  Called from init()
         *
         *  @param config the config Object to read initialization
         *                information from.
         *  @param bufferSecs number of seconds to buffer audio for
         *  @exception Exception
         */
        void
        configIceCast (  const Config   & config,
                         unsigned int     bufferSecs  )     throw ( Exception );

        /**
         *  Look for the icecast2 stream outputs from the config file.
         *  Called from init()
         *
         *  @param config the config Object to read initialization
         *                information from.
         *  @param bufferSecs number of seconds to buffer audio for
         *  @exception Exception
         */
        void
        configIceCast2 (  const Config   & config,
                          unsigned int     bufferSecs  )    throw ( Exception );

        /**
         *  Look for the shoutcast stream outputs from the config file.
         *  Called from init()
         *
         *  @param config the config Object to read initialization
         *                information from.
         *  @param bufferSecs number of seconds to buffer audio for
         *  @exception Exception
         */
        void
        configShoutCast (   const Config   & config,
                            unsigned int     bufferSecs )   throw ( Exception );

        /**
         *  Look for file outputs from the config file.
         *  Called from init()
         *
         *  @param config the config Object to read initialization
         *                information from.
         *  @exception Exception
         */
        void
        configFileCast  (   const Config   & config )
                                                            throw ( Exception );

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

        /**
         *  Signal to each sink we have that they need to cut what they are
         *  doing, and start again. For FileSinks, this usually means to
         *  save the archive file recorded so far, and start a new archive
         *  file.
         */
        virtual void
        cut ( void )                                throw ();

};


/* ================================================= external data structures */


/* ====================================================== function prototypes */



#endif  /* DARK_ICE_H */

