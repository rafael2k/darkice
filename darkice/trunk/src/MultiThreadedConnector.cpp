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
    pthread_mutex_init( &mutexConsume, 0);
    pthread_cond_init( &condConsume, 0);
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

    pthread_cond_destroy( &condConsume);
    pthread_mutex_destroy( &mutexConsume);
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
    mutexConsume    = connector.mutexConsume;
    condConsume     = connector.condConsume;

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
        mutexConsume    = connector.mutexConsume;
        condConsume     = connector.condConsume;

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
 reportEvent( 8, "Connector :: open starting thread ", i);
        ThreadData    * threadData = threads + i;

        threadData->connector = this;
        threadData->ixSink    = i;
        threadData->isDone    = true;
        if ( pthread_create( &(threadData->thread),
                             &threadAttr,
                             ThreadData::threadFunction,
                             threadData ) ) {
 reportEvent( 8, "Connector :: open couldn't start thread ", i);
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

reportEvent( 8, "Connector :: transfer, mutexProduce, enter lock");
            pthread_mutex_lock( &mutexProduce);
reportEvent( 8, "Connector :: transfer, mutexProduce, lock");
            dataSize = source->read( dataBuffer, bufSize);
reportEvent( 8, "Connector :: transfer, read", dataSize);
            b       += dataSize;

            /* check for EOF */
            if ( dataSize == 0 ) {
                reportEvent( 3, "Connector :: transfer, EOF");
                break;
            }

            for ( i = 0; i < numSinks; ++i ) {
                threads[i].isDone = false;
            }

            /* tell sink threads that there is some data available */
reportEvent( 8, "Connector :: transfer, mutexProduce, broadcast");
            pthread_cond_broadcast( &condProduce);
reportEvent( 8, "Connector :: transfer, mutexProduce, broadcasted");

            /* wait for all sink threads to get done with this data */
            while ( true ) {
                for ( i = 0; i < numSinks && threads[i].isDone; ++i );
                if ( i == numSinks ) {
                    break;
                }
reportEvent( 8, "Connector :: transfer, mutexConsume, waiting");
                pthread_cond_wait( &condProduce, &mutexProduce);
reportEvent( 8, "Connector :: transfer, mutexConsume, waking");
            }
            pthread_mutex_unlock( &mutexProduce);
reportEvent( 8, "Connector :: transfer, mutexProduce, exit lock");

            /* wait for all sink threads to process the presented data */
/*
reportEvent( 8, "Connector :: transfer, mutexConsume, enter lock");
            pthread_mutex_lock( &mutexConsume);
reportEvent( 8, "Connector :: transfer, mutexConsume, lock");
            while ( true ) {
                for ( i = 0; i < numSinks && threads[i].isDone; ++i );
                if ( i == numSinks ) {
                    break;
                }
reportEvent( 8, "Connector :: transfer, mutexConsume, waiting");
                pthread_cond_wait( &condConsume, &mutexConsume);
reportEvent( 8, "Connector :: transfer, mutexConsume, waking");
            }
            consumeCount = 0;
reportEvent( 8, "Connector :: transfer, mutexConsume, unlock");
            pthread_mutex_unlock( &mutexConsume);
reportEvent( 8, "Connector :: transfer, mutexConsume, exit lock");
*/
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
reportEvent( 8, "Connector :: sinkThread, mutexProduce, enter lock ", ixSink);
        pthread_mutex_lock( &mutexProduce);
reportEvent( 8, "Connector :: sinkThread, mutexProduce, lock ", ixSink);
        while ( running && threadData->isDone ) {
reportEvent( 8, "Connector :: sinkThread, mutexProduce, wait ", ixSink);
            pthread_cond_wait( &condProduce, &mutexProduce);
reportEvent( 8, "Connector :: sinkThread, mutexProduce, wake ", ixSink);
        }
        if ( !running ) {
            break;
        }

        if ( sink->canWrite( 0, 0) ) {
            unsigned int        written;
            written = sink->write( dataBuffer, dataSize);
/*            
            unsigned int        written;
            written = sink->write( dataBuffer, dataSize);
reportEvent( 8, "Connector :: sinkThread, written ", written);
*/
        } else {
reportEvent( 3, "Connector :: sinkThread MAYDAY MAYDAY can't write ", ixSink);
            /* don't care if we can't write */
        }
        threadData->isDone = true;
reportEvent( 8, "Connector :: sinkThread, condProduce, broadcast ", ixSink);
        pthread_cond_broadcast( &condProduce);
reportEvent( 8, "Connector :: sinkThread, condProduce, broadcasted ", ixSink);
reportEvent( 8, "Connector :: sinkThread, mutexProduce, unlock", ixSink);
        pthread_mutex_unlock( &mutexProduce);
reportEvent( 8, "Connector :: sinkThread, mutexProduce, exit lock", ixSink);

        /* signal that we have read the data */
/*
reportEvent( 8, "Connector :: sinkThread, mutexConsume, enter lock");
        pthread_mutex_lock( &mutexConsume);
reportEvent( 8, "Connector :: sinkThread, mutexConsume, lock");
reportEvent( 8, "Connector :: sinkThread, mutexConsume, signal");
        pthread_cond_signal( &condConsume);
reportEvent( 8, "Connector :: sinkThread, mutexConsume, unlock");
        pthread_mutex_unlock( &mutexConsume);
reportEvent( 8, "Connector :: sinkThread, mutexConsume, exit lock");
*/
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
threadData->connector->reportEvent( 8,
                  "ThreadData :: threadFunction thread starts ",
                 (threadData->ixSink) );
threadData->connector->reportEvent( 8,
                  "ThreadData :: threadFunction numSinks ",
                  threadData->connector->numSinks);
for ( unsigned int i = 0; i < threadData->connector->numSinks; ++i ) {
threadData->connector->reportEvent( 8,
                  "ThreadData :: threadFunction thread ", i,
                  " ixSink ", threadData->connector->threads[i].ixSink);
}
    threadData->connector->sinkThread( threadData->ixSink);

    return 0;
}


/*------------------------------------------------------------------------------
 
  $Source$

  $Log$
  Revision 1.1  2002/10/19 12:25:47  darkeye
  changed internals so that now each encoding/server connection is
  a separate thread


  
------------------------------------------------------------------------------*/

