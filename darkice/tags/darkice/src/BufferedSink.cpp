/*------------------------------------------------------------------------------

   Copyright (c) 2000 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : BufferedSink.cpp
   Version  : $Revision$
   Author   : $Author$
   Location : $Source$
   
   Abstract : 

     A Sink with a First-In First-Out buffer
     This buffer can always be written to, it overwrites any
     data contained if needed
     The class is not thread-safe

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
     as published by the Free Software Foundation; either version 2
     of the License, or (at your option) any later version.
    
     This program is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of 
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
     GNU General Public License for more details.
    
     You should have received a copy of the GNU General Public License
     along with this program; if not, write to the Free Software
     Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307,
     USA.

------------------------------------------------------------------------------*/

/* ============================================================ include files */

#include <string.h>

#include "Exception.h"
#include "BufferedSink.h"


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
BufferedSink :: init (  Sink          * sink,
                        unsigned int    size )          throw ( Exception )
{
    if ( !sink ) {
        throw Exception( __FILE__, __LINE__, "no sink");
    }

    this->sink       = sink;                    // create a reference
    this->bufferSize = size;
    this->buffer     = new unsigned char[bufferSize];
    this->bufferEnd  = buffer + bufferSize;
    this->inp        = buffer;
    this->outp       = buffer;
}


/*------------------------------------------------------------------------------
 *  Copy Constructor
 *----------------------------------------------------------------------------*/
BufferedSink :: BufferedSink (  const BufferedSink &  buffer )
                                                        throw ( Exception )
{
    init( buffer.sink.get(), buffer.bufferSize);

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
        init( buffer.sink.get(), buffer.bufferSize);
        
        memcpy( this->buffer, buffer.buffer, this->bufferSize);
    }

    return *this;
}


/*------------------------------------------------------------------------------
 *  Store bufferSize bytes into the buffer
 *  All data is consumed. The return value is less then bufferSize only
 *  if the BufferedSink's internal buffer is smaller than bufferSize,
 *  thus can't hold that much
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
    
    /* cut the front of the supplied buffer if it wouldn't fit */
    if ( bufferSize > this->bufferSize ) {
        size = this->bufferSize - 1;
        buf += bufferSize - size;
    } else {
        size = bufferSize;
    }

    /* copy the data into the buffer */
    i = bufferEnd - inp;
    if ( size <= i ) {
        /* the place between inp and bufferEnd is
         * big enough to hold the data */
        
        memcpy( inp, buf, size);
        inp = slidePointer( inp, size);

        /* adjust outp, lose the data that was overwritten, if any */
        if ( outp > oldInp && outp <= inp ) {
            outp = slidePointer( inp, 1);
        }

    } else {
        /* the place between inp and bufferEnd is not
         * big enough to hold the data
         * writing will take place in two turns, once from
         * inp -> bufferEnd, then from buffer -> */

        memcpy( inp, buf, i);
        i = size - i;
        memcpy( this->buffer, buf, i);
        inp = slidePointer( this->buffer, i);
        
        /* adjust outp, lose the data that was overwritten, if any */
        if ( outp <= oldInp ) {
            if ( outp < inp ) {
                outp = slidePointer( inp, 1);
            }
        } else {
            outp = slidePointer( inp, 1);
        }
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
    
    if ( !buf ) {
        throw Exception( __FILE__, __LINE__, "buf is null");
    }

    if ( !isOpen() ) {
        return 0;
    }

    /* try to write data from the buffer first, if any */
    if ( inp != outp ) {
        unsigned int    size;

        if ( outp > inp ) {
            /* valuable data is between outp -> bufferEnd and buffer -> inp
             * try to write the outp -> bufferEnd
             * the rest will be written in the next if */

            size    = bufferEnd - outp;
            length  = sink->write( outp, size);
            outp    = slidePointer( outp, length);
        }

        if ( outp < inp ) {
            /* valuable data is between outp and inp
             * if the previous if wrote all data from the end
             * this part will write the rest */
            
            size    = inp - outp;
            length  = sink->write( outp, size);
            outp    = slidePointer( outp, length);
        }
    }

    /* the internal buffer is empty, try to write the fresh data */
    length = inp == outp ? sink->write( buf, len) : 0;

    if ( length < len ) {
        /* if not all fresh could be written, store the remains */

        unsigned char     * b = (unsigned char *) buf;

        store( b + length, len - length);
    }

    /* tell them we ate everything */
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


/*------------------------------------------------------------------------------
 
  $Source$

  $Log$
  Revision 1.1  2000/11/05 10:05:48  darkeye
  Initial revision

  
------------------------------------------------------------------------------*/

