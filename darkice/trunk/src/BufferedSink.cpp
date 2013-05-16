/*------------------------------------------------------------------------------

   Copyright (c) 2000-2007 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : BufferedSink.cpp
   Version  : $Revision: 474 $
   Author   : $Author: rafael@riseup.net $
   Location : $HeadURL$
   
     the buffer is filled like this:
     
     buffer                                         bufferEnd
      |                                                    |
      +----------+--------------------------+--------------+
                 |<---- valid data -------->|
                outp                       inp 

     buffer                                         bufferEnd
      |                                                    |
      +----------------+--------------+--------------------+
      -- valid data -->|              |--- valid data ----->
                      inp            outp



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

#ifdef HAVE_STRING_H
#include <string.h>
#else
#error need string.h
#endif


#include "Exception.h"
#include "BufferedSink.h"


/* ===================================================  local data structures */


/* ================================================  local constants & macros */

/*------------------------------------------------------------------------------
 *  File identity
 *----------------------------------------------------------------------------*/
static const char fileid[] = "$Id: BufferedSink.cpp 474 2010-05-10 01:18:15Z rafael@riseup.net $";


/* ===============================================  local function prototypes */


/* =============================================================  module code */

/*------------------------------------------------------------------------------
 *  Initialize the object
 *----------------------------------------------------------------------------*/
void
BufferedSink :: init (  Sink          * sink,
                        unsigned int    size,
                        unsigned int    chunkSize )     throw ( Exception )
{
    if ( !sink ) {
        throw Exception( __FILE__, __LINE__, "no sink");
    }

    this->sink         = sink;                    // create a reference
    this->chunkSize    = chunkSize ? chunkSize : 1;
    this->bufferSize   = size;
    // make bufferSize a multiple of chunkSize
    this->bufferSize  -= this->bufferSize % this->chunkSize;
    this->peak         = 0;
    this->misalignment = 0;
    this->buffer       = new unsigned char[bufferSize];
    this->bufferEnd    = buffer + bufferSize;
    this->inp          = buffer;
    this->outp         = buffer;
}


/*------------------------------------------------------------------------------
 *  Copy Constructor
 *----------------------------------------------------------------------------*/
BufferedSink :: BufferedSink (  const BufferedSink &  buffer )
                                                        throw ( Exception )
{
    init( buffer.sink.get(), buffer.bufferSize, buffer.chunkSize);

    this->peak         = buffer.peak;
    this->misalignment = buffer.misalignment;
    memcpy( this->buffer, buffer.buffer, this->bufferSize);
}


/*------------------------------------------------------------------------------
 *  De-initalize the object
 *----------------------------------------------------------------------------*/
void
BufferedSink :: strip ( void )                      throw ( Exception )
{
    if ( isOpen() ) {
        close();
    }

    sink = 0;                                   // delete the reference
    delete buffer;
}


/*------------------------------------------------------------------------------
 *  Assignment operator
 *----------------------------------------------------------------------------*/
BufferedSink &
BufferedSink :: operator= (     const BufferedSink &  buffer )
                                                    throw ( Exception )
{
    if ( this != &buffer ) {
        strip();
        Sink::operator=( buffer );
        init( buffer.sink.get(), buffer.bufferSize, buffer.chunkSize);
        
        this->peak         = buffer.peak;
        this->misalignment = buffer.misalignment;
        memcpy( this->buffer, buffer.buffer, this->bufferSize);
    }

    return *this;
}


/*------------------------------------------------------------------------------
 *  Store bufferSize bytes into the buffer
 *  All data is consumed. The return value is less then bufferSize only
 *  if the BufferedSink's internal buffer is smaller than bufferSize,
 *  thus can't hold that much
 *  The data to be stored is treated as parts with chunkSize size
 *  Only full chunkSize sized parts are stored
 *----------------------------------------------------------------------------*/
unsigned int
BufferedSink :: store (     const void    * buffer,
                            unsigned int    bufferSize )    throw ( Exception )
{
    const unsigned char   * buf;
    unsigned int            size;
    unsigned int            i;
    unsigned char         * oldInp;

    if ( !buffer ) {
        throw Exception( __FILE__, __LINE__, "buffer is null");
    }

    if ( !bufferSize ) {
        return 0;
    }

    oldInp = inp;
    buf    = (const unsigned char *) buffer;
    
    // adjust so it is a multiple of chunkSize
    bufferSize -= bufferSize % chunkSize;

    // cut the front of the supplied buffer if it wouldn't fit
    if ( bufferSize > this->bufferSize ) {
        size  = this->bufferSize - 1;
        size -= size % chunkSize;       // keep it a multiple of chunkSize
        buf  += bufferSize - size;
    } else {
        size = bufferSize;
    }

    // copy the data into the buffer
    i = bufferEnd - inp;
    if ( (i % chunkSize) != 0 ) {
        throw Exception( __FILE__, __LINE__, "copy quantity not aligned", i);
    }

    if ( size <= i ) {
        // the place between inp and bufferEnd is
        // big enough to hold the data
        
        memcpy( inp, buf, size);
        inp = slidePointer( inp, size);

        // adjust outp, lose the data that was overwritten, if any
        if ( outp > oldInp && outp <= inp ) {
            outp = slidePointer( inp, chunkSize);
        }

    } else {
        // the place between inp and bufferEnd is not
        // big enough to hold the data
        // writing will take place in two turns, once from
        // inp -> bufferEnd, then from buffer ->

        memcpy( inp, buf, i);
        i = size - i;
        if ( (i % chunkSize) != 0 ) {
            throw Exception(__FILE__, __LINE__, "copy quantity not aligned", i);
        }
        memcpy( this->buffer, buf, i);
        inp = slidePointer( this->buffer, i);
        
        // adjust outp, lose the data that was overwritten, if any
        if ( outp <= oldInp ) {
            if ( outp < inp ) {
                outp = slidePointer( inp, chunkSize);
            }
        } else {
            outp = slidePointer( inp, chunkSize);
        }
    }

    updatePeak();

    if ( ((inp - this->buffer) % chunkSize) != 0 ) {
        throw Exception( __FILE__, __LINE__,
                         "inp not aligned", inp - this->buffer);
    }
    if ( ((outp - this->buffer) % chunkSize) != 0 ) {
        throw Exception( __FILE__, __LINE__,
                         "outp not aligned", outp - this->buffer);
    }

    return size;
}


/*------------------------------------------------------------------------------
 *  Write some data to the sink
 *  if len == 0, try to flush the buffer
 *----------------------------------------------------------------------------*/
unsigned int
BufferedSink :: write (    const void    * buf,
                           unsigned int    len )       throw ( Exception )
{
    unsigned int    length;
    unsigned int    soFar;
    unsigned char * b = (unsigned char *) buf;

    if ( !buf ) {
        throw Exception( __FILE__, __LINE__, "buf is null");
    }

    if ( !isOpen() ) {
        return 0;
    }

    if ( !align() ) {
        return 0;
    }

    // make it a multiple of chunkSize
    len -= len % chunkSize;

    // try to write data from the buffer first, if any
    if ( inp != outp ) {
        unsigned int    size  = 0;
        unsigned int    total = 0;

        if ( outp > inp ) {
            // valuable data is between outp -> bufferEnd and buffer -> inp
            // try to write the outp -> bufferEnd
            // the rest will be written in the next if

            size    = bufferEnd - outp - 1;
            size   -= size % chunkSize;
            soFar   = 0;

            while ( outp > inp && soFar < size && sink->canWrite( 0, 0) ) {
                length  = sink->write( outp + soFar, size - soFar);
                outp    = slidePointer( outp, length);
                soFar  += length;
            }

            total += soFar;
        }

        if ( outp < inp ) {
            // valuable data is between outp and inp
            // in the previous if wrote all data from the end
            // this part will write the rest

            size    = inp - outp;
            soFar   = 0;

            while ( soFar < size && sink->canWrite( 0, 0) ) {
                length  = sink->write( outp + soFar, size - soFar);
                outp    = slidePointer( outp, length);
                soFar  += length;
            }

            total += soFar;
        }

        while ( (outp - buffer) % chunkSize ) {
            slidePointer( outp, 1);
        }

        // calulate the misalignment to chunkSize boundaries
        misalignment = (chunkSize - (total % chunkSize)) % chunkSize;
    }

    if ( !align() ) {
        return 0;
    }

    // the internal buffer is empty, try to write the fresh data
    soFar = 0;
    if ( inp == outp ) { 
        while ( soFar < len && sink->canWrite( 0, 0) ) {
	    try {
	        soFar += sink->write( b + soFar, len - soFar);
	    } catch (Exception &e) {
	        reportEvent(3,"Exception caught in BufferedSink :: write3\n");
		throw; /* up a level */
	    }
	}
    }
    length = soFar;

    // calulate the misalignment to chunkSize boundaries
    misalignment = (chunkSize - (length % chunkSize)) % chunkSize;

    if ( length < len ) {
        // if not all fresh could be written, store the remains

        store( b + length, len - length);
    }

    // tell them we ate everything up to chunkSize alignment
    return len;
}


/*------------------------------------------------------------------------------
 *  Close the sink, lose all pending data
 *----------------------------------------------------------------------------*/
void
BufferedSink :: close ( void )                      throw ( Exception )
{
    if ( !isOpen() ) {
        return;
    }

    flush();
    sink->close();
    inp = outp = buffer;
}

