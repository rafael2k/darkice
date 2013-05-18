/*------------------------------------------------------------------------------

   Copyright (c) 2000-2007 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : MultiThreadedConnector.h
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
#ifndef MULTI_THREADED_CONNECTOR_H
#define MULTI_THREADED_CONNECTOR_H

#ifndef __cplusplus
#error This is a C++ include file
#endif


/* ============================================================ include files */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

// check for __NetBSD__ because it won't be found by AC_CHECK_HEADER on NetBSD
// as pthread.h is in /usr/pkg/include, not /usr/include
#if defined( HAVE_PTHREAD_H ) || defined( __NetBSD__ )
#include <pthread.h>
#else
#error need pthread.h
#endif

#include "Referable.h"
#include "Ref.h"
#include "Reporter.h"
#include "Source.h"
#include "Sink.h"
#include "Connector.h"


/* ================================================================ constants */


/* =================================================================== macros */


/* =============================================================== data types */

/**
 *  Connects a source to one or more sinks, using a multi-threaded
 *  producer - consumer approach.
 *
 *  @author  $Author$
 *  @version $Revision$
 */
class MultiThreadedConnector : public virtual Connector
{
    private:

        /**
         *  Helper class to collect information for starting threads.
         */
        class ThreadData
        {
            public:
                /**
                 *  The connector starting the thread
                 */
                MultiThreadedConnector    * connector;

                /**
                 *  The index of the sink this thread writes to.
                 */
                unsigned int                ixSink;

                /**
                 *  The POSIX thread itself.
                 */
                pthread_t                   thread;

                /**
                 *  Marks if the thread is accepting data.
                 */
                bool                        accepting;

                /**
                 *  Marks if the thread has processed the last batch
                 *  of data.
                 */
                bool                        isDone;

                /**
                 *  A flag to show that the sink should be made to cut in the
                 *  next iteration.
                 */
                bool                    cut;

                /**
                 *  Default constructor.
                 */
                inline
                ThreadData()
                {
                    this->connector = 0;
                    this->ixSink    = 0;
                    this->thread    = 0;
                    this->accepting = false;
                    this->isDone    = false;
                    this->cut       = false;
                }

                /**
                 *  The thread function.
                 *
                 *  @param param thread parameter, a pointer to a
                 *               ThreadData
                 *  @return nothing
                 */
                static void *
                threadFunction( void      * param );
        };
        
        /**
         *  The mutex of this object.
         */
        pthread_mutex_t         mutexProduce;

        /**
         *  The conditional variable for presenting new data.
         */
        pthread_cond_t          condProduce;

        /**
         *  The thread attributes.
         */
        pthread_attr_t          threadAttr;

        /**
         *  The threads for the sinks.
         */
        ThreadData            * threads;

        /**
         *  Signal if we're running or not, so the threads no if to stop.
         */
        bool                    running;

        /**
         *  Flag to show if the connector should try to reconnect if
         *  the connection is dropped on the other side.
         */
        bool                    reconnect;

        /**
         *  The buffer of information presented to each thread.
         */
        unsigned char         * dataBuffer;

        /**
         *  The amount of information presented to each thread.
         */
        unsigned int            dataSize;

        /**
         *  Initialize the object.
         *
         *  @param reconnect flag to indicate if the connector should
         *                   try to reconnect if the connection was
         *                   dropped by the other end
         *  @exception Exception
         */
        void
        init ( bool     reconnect )                 throw ( Exception );

        /**
         *  De-initialize the object.
         *
         *  @exception Exception
         */
        void
        strip ( void )                              throw ( Exception );

    protected:

        /**
         *  Default constructor. Always throws an Exception.
         *
         *  @exception Exception
         */
        inline
        MultiThreadedConnector ( void )                 throw ( Exception )
        {
            throw Exception( __FILE__, __LINE__);
        }


    public:

        /**
         *  Constructor based on a Source.
         *
         *  @param source the source to connect to the sinks.
         *  @param reconnect flag to indicate if the connector should
         *                   try to reconnect if the connection was
         *                   dropped by the other end
         *  @exception Exception
         */
        inline
        MultiThreadedConnector (    Source        * source,
                                    bool            reconnect )
                                                            throw ( Exception )
                    : Connector( source )
        {
            init(reconnect);
        }

        /**
         *  Constructor based on a Source and a Sink.
         *
         *  @param source the source to connect to the sinks.
         *  @param sink a sink to connect to the source.
         *  @param reconnect flag to indicate if the connector should
         *                   try to reconnect if the connection was
         *                   dropped by the other end
         *  @exception Exception
         */
        inline
        MultiThreadedConnector ( Source            * source,
                                 Sink              * sink,
                                 bool                reconnect )
                                                            throw ( Exception )
                    : Connector( source, sink)
        {
            init(reconnect);
        }

        /**
         *  Copy constructor.
         *
         *  @param connector the object to copy.
         *  @exception Exception
         */
        MultiThreadedConnector ( const MultiThreadedConnector &   connector )
                                                            throw ( Exception );
        
        /**
         *  Destructor.
         *
         *  @exception Exception
         */
        inline virtual
        ~MultiThreadedConnector( void )                     throw ( Exception )
        {
            strip();
        }

        /**
         *  Assignment operator.
         *
         *  @param connector the object to assign to this one.
         *  @return a reference to this object.
         *  @exception Exception
         */
        virtual MultiThreadedConnector &
        operator= ( const MultiThreadedConnector &   connector )
                                                            throw ( Exception );

        /**
         *  Open the connector. Opens the Source and the Sinks if necessary.
         *
         *  @return true if opening was successful, false otherwise.
         *  @exception Exception
         */
        virtual bool
        open ( void )                                   throw ( Exception );

        /**
         *  Transfer a given amount of data from the Source to all the
         *  Sinks attached.
         *  If an attached Sink closes or encounteres an error during the
         *  process, it is detached and the function carries on with the
         *  rest of the Sinks. If no Sinks remain, or an error is encountered
         *  with the Source, the function returns prematurely.
         *
         *  @param bytes the amount of data to transfer, in bytes.
         *               If 0, transfer forever.
         *  @param bufSize the size of the buffer to use for transfering.
         *                 This amount of data is read from the Source and
         *                 written to each Sink on each turn.
         *  @param sec the number of seconds to wait for the Source to have
         *             data available in each turn, and the number of seconds
         *             to wait for the Sinks to accept data.
         *  @param usec the number of micros seconds to wait for the Source to
         *             have data available in each turn, and the number of
         *             micro seconds to wait for the Sinks to accept data.
         *  @return the number of bytes read from the Source.
         *  @exception Exception
         */
        virtual unsigned int
        transfer (  unsigned long       bytes,
                    unsigned int        bufSize,
                    unsigned int        sec,
                    unsigned int        usec )          throw ( Exception );

        /**
         *  Signal to each sink we have that they need to cut what they are
         *  doing, and start again. For FileSinks, this usually means to
         *  save the archive file recorded so far, and start a new archive
         *  file.
         */
        virtual void
        cut ( void )                                throw ();

        /**
         *  Close the Connector. The Source and all Sinks are closed.
         *
         *  @exception Exception
         */
        virtual void
        close ( void )                                  throw ( Exception );

        /**
         *  This is the worker function for each thread.
         *  This function has to return fast
         *
         *  @param ixSink the index of the sink this thread works on.
         */
        void
        sinkThread( int     ixSink );
};


/* ================================================= external data structures */


/* ====================================================== function prototypes */



#endif  /* MULTI_THREADED_CONNECTOR_H */

