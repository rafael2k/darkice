/*------------------------------------------------------------------------------

   Copyright (c) 2000 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : MultiThreadedConnector.cpp
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

#include "Exception.h"
#include "MultiThreadedConnector.h"


/* ===================================================  local data structures */


/* ================================================  local constants & macros */

/*------------------------------------------------------------------------------
 *  File identity
 *----------------------------------------------------------------------------*/
static const char fileid[] = "$Id$";


/* ===============================================  local function prototypes */


/* =============================================================  module code */

/*------------------------------------------------------------------------------
 *  Initialize the object
 *----------------------------------------------------------------------------*/
void
MultiThreadedConnector :: init ( void )                 throw ( Exception )
{
    pthread_mutex_init( &mutexProduce, 0);
    pthread_cond_init( &condProduce, 0);
    threads = 0;
}


/*------------------------------------------------------------------------------
 *  De-initialize the object
 *----------------------------------------------------------------------------*/
void
MultiThreadedConnector :: strip ( void )                throw ( Exception )
{
    if ( threads ) {
        delete[] threads;
        threads = 0;
    }

    pthread_cond_destroy( &condProduce);
    pthread_mutex_destroy( &mutexProduce);
}


/*------------------------------------------------------------------------------
 *  Constructor
 *----------------------------------------------------------------------------*/
MultiThreadedConnector :: MultiThreadedConnector (
                                const MultiThreadedConnector &   connector )
                                                            throw ( Exception )
            : Connector( connector)
{
    mutexProduce    = connector.mutexProduce;
    condProduce     = connector.condProduce;

    if ( threads ) {
        delete[] threads;
    }
    threads = new ThreadData[numSinks];
    for ( unsigned int  i = 0; i < numSinks; ++i ) {
        threads[i] = connector.threads[i];
    }
}


/*------------------------------------------------------------------------------
 *  Assignment operator
 *----------------------------------------------------------------------------*/
MultiThreadedConnector &
MultiThreadedConnector :: operator= ( const MultiThreadedConnector & connector )
                                                            throw ( Exception )
{
    if ( this != &connector ) {
        Connector::operator=( connector);

        mutexProduce    = connector.mutexProduce;
        condProduce     = connector.condProduce;

        if ( threads ) {
            delete[] threads;
        }
        threads = new ThreadData[numSinks];
        for ( unsigned int  i = 0; i < numSinks; ++i ) {
            threads[i] = connector.threads[i];
        }
    }

    return *this;
}


/*------------------------------------------------------------------------------
 *  Open the source and all the sinks if needed
 *  Create the sink threads
 *----------------------------------------------------------------------------*/
bool
MultiThreadedConnector :: open ( void )                     throw ( Exception )
{
    unsigned int        i;

    if ( !Connector::open() ) {
        return false;
    }

    running = true;

    pthread_attr_init( &threadAttr);
    pthread_attr_setdetachstate( &threadAttr, PTHREAD_CREATE_JOINABLE);

    threads = new ThreadData[numSinks];
    for ( i = 0; i < numSinks; ++i ) {
        ThreadData    * threadData = threads + i;

        threadData->connector = this;
        threadData->ixSink    = i;
        threadData->accepting = true;
        threadData->isDone    = true;
        if ( pthread_create( &(threadData->thread),
                             &threadAttr,
                             ThreadData::threadFunction,
                             threadData ) ) {
            break;
        }
    }

    /* if could not create all, delete the ones created */
    if ( i < numSinks ) {
        unsigned int    j;

        /* signal to stop for all running threads */
        pthread_mutex_lock( &mutexProduce);
        running = false;
        pthread_cond_broadcast( &condProduce);
        pthread_mutex_unlock( &mutexProduce);

        for ( j = 0; j < i; ++j ) {
            pthread_join( threads[j].thread, 0);
        }

        delete[] threads;
        threads = 0;

        return false;
    }

    return true;
}


/*------------------------------------------------------------------------------
 *  Transfer some data from the source to the sink
 *----------------------------------------------------------------------------*/
unsigned int
MultiThreadedConnector :: transfer ( unsigned long       bytes,
                                     unsigned int        bufSize,
                                     unsigned int        sec,
                                     unsigned int        usec )
                                                            throw ( Exception )
{   
    unsigned int        b;

    if ( numSinks == 0 ) {
        return 0;
    }

    if ( bufSize == 0 ) {
        return 0;
    }

    dataBuffer   = new unsigned char[bufSize];
    dataSize     = 0;

    reportEvent( 6, "MultiThreadedConnector :: tranfer, bytes", bytes);

    for ( b = 0; !bytes || b < bytes; ) {
        if ( source->canRead( sec, usec) ) {
            unsigned int        i;

            pthread_mutex_lock( &mutexProduce);
            dataSize = source->read( dataBuffer, bufSize);
            b       += dataSize;

            /* check for EOF */
            if ( dataSize == 0 ) {
                reportEvent( 3, "Connector :: transfer, EOF");
                break;
            }

            for ( i = 0; i < numSinks; ++i ) {
                if ( threads[i].accepting ) {
                    threads[i].isDone = false;
                }
            }

            /* tell sink threads that there is some data available */
            pthread_cond_broadcast( &condProduce);

            /* wait for all sink threads to get done with this data */
            while ( true ) {
                for ( i = 0; i < numSinks; ++i ) {
                    if ( threads[i].accepting && !threads[i].isDone ) {
                        break;
                    }
                }
                if ( i == numSinks ) {
                    break;
                }
                pthread_cond_wait( &condProduce, &mutexProduce);
            }
            pthread_mutex_unlock( &mutexProduce);
        } else {
            reportEvent( 3, "Connector :: transfer, can't read");
            break;
        }
    }

    delete[] dataBuffer;
    return b;
}


/*------------------------------------------------------------------------------
 *  The function for each thread.
 *  Read the presented data
 *----------------------------------------------------------------------------*/
void
MultiThreadedConnector :: sinkThread( int       ixSink )
{
    ThreadData    * threadData = &threads[ixSink];
    Sink          * sink       = sinks[ixSink].get();

    while ( running ) {
        /* wait for some data to become available */
        pthread_mutex_lock( &mutexProduce);
        while ( running && threadData->isDone ) {
            pthread_cond_wait( &condProduce, &mutexProduce);
        }
        if ( !running ) {
            break;
        }

        if ( sink->canWrite( 0, 0) ) {
            try {
                sink->write( dataBuffer, dataSize);
            } catch ( Exception     & e ) {
                // something wrong. don't accept more data, try to
                // reopen the sink
                threadData->accepting = false;
                pthread_mutex_unlock( &mutexProduce);

                do {
                    try {
                        sink->close();
                        sink->open();
                    } catch ( Exception   & e ) {
                        // don't care, just try and try again
                    }
                    // TODO: wait some time every cycle, so as not to
                    //       overload the system
                } while ( !sink->isOpen() );

                // all OK, accept data, act as if we're done
                pthread_mutex_lock( &mutexProduce);
                threadData->accepting = true;
                threadData->isDone = true;
            }
        } else {
            reportEvent( 4,
                         "MultiThreadedConnector :: sinkThread can't write ",
                         ixSink);
            /* don't care if we can't write */
        }
        threadData->isDone = true;
        pthread_cond_broadcast( &condProduce);
        pthread_mutex_unlock( &mutexProduce);
    }
}


/*------------------------------------------------------------------------------
 *  Stop the treads
 *  Close the source and all the sinks if needed
 *----------------------------------------------------------------------------*/
void
MultiThreadedConnector :: close ( void )                    throw ( Exception )
{
    unsigned int    i;

    /* signal to stop for all threads */
    pthread_mutex_lock( &mutexProduce);
    running = false;
    pthread_cond_broadcast( &condProduce);
    pthread_mutex_unlock( &mutexProduce);

    for ( i = 0; i < numSinks; ++i ) {
        pthread_join( threads[i].thread, 0);
    }
    pthread_attr_destroy( &threadAttr);

    Connector::close();
}


/*------------------------------------------------------------------------------
 *  The thread function
 *----------------------------------------------------------------------------*/
void *
MultiThreadedConnector :: ThreadData :: threadFunction( void  * param )
{
    ThreadData     * threadData = (ThreadData*) param;
    threadData->connector->sinkThread( threadData->ixSink);

    return 0;
}


/*------------------------------------------------------------------------------
 
  $Source$

  $Log$
  Revision 1.2  2002/10/19 13:35:21  darkeye
  when a connection is dropped, DarkIce tries to reconnect, indefinitely
  removed extreme event reporting for thread-related events

  Revision 1.1  2002/10/19 12:25:47  darkeye
  changed internals so that now each encoding/server connection is
  a separate thread


  
------------------------------------------------------------------------------*/

