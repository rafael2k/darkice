/*------------------------------------------------------------------------------

   Copyright (c) 2000 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : MultiThreadedConnector.h
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
#ifndef MULTI_THREADED_CONNECTOR_H
#define MULTI_THREADED_CONNECTOR_H

#ifndef __cplusplus
#error This is a C++ include file
#endif


/* ============================================================ include files */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_PTHREAD_H
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
                 *  Marks if the thread has processed the last batch
                 *  of data.
                 */
                bool                        isDone;

                /**
                 *  Default constructor.
                 */
                inline
                ThreadData()
                {
                    this->connector = 0;
                    this->ixSink    = 0;
                    this->thread    = 0;
                    this->isDone    = false;
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
         *  The mutex of this object.
         */
        pthread_mutex_t         mutexConsume;

        /**
         *  The conditional variable for consuming data.
         */
        pthread_cond_t          condConsume;

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
         *  @exception Exception
         */
        void
        init ( void )                               throw ( Exception );

        /**
         *  De-initialize the object.
         *
         *  @exception Exception
         */
        void
        strip ( void )                              throw ( Exception );

        /**
         *  This is the function for each thread.
         *  This function has to return fast
         *
         *  @param ixSink the index of the sink this thread works on.
         */
        void
        sinkThread( int     ixSink );

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
         *  @exception Exception
         */
        inline
        MultiThreadedConnector (    Source        * source )
                                                            throw ( Exception )
                    : Connector( source )
        {
            init();
        }

        /**
         *  Constructor based on a Source and a Sink.
         *
         *  @param source the source to connect to the sinks.
         *  @param sink a sink to connect to the source.
         *  @exception Exception
         */
        inline
        MultiThreadedConnector ( Source            * source,
                                 Sink              * sink )
                                                            throw ( Exception )
                    : Connector( source, sink)
        {
            init();
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
         *  Close the Connector. The Source and all Sinks are closed.
         *
         *  @exception Exception
         */
        virtual void
        close ( void )                                  throw ( Exception );
};


/* ================================================= external data structures */


/* ====================================================== function prototypes */



#endif  /* MULTI_THREADED_CONNECTOR_H */


/*------------------------------------------------------------------------------
 
  $Source$

  $Log$
  Revision 1.1  2002/10/19 12:25:47  darkeye
  changed internals so that now each encoding/server connection is
  a separate thread


  
------------------------------------------------------------------------------*/

