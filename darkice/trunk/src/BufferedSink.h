/*------------------------------------------------------------------------------

   Copyright (c) 2000-2007 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : BufferedSink.h
   Version  : $Revision: 474 $
   Author   : $Author: rafael@riseup.net $
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
#ifndef BUFFERED_SINK_H
#define BUFFERED_SINK_H

#ifndef __cplusplus
#error This is a C++ include file
#endif


/* ============================================================ include files */

#include "Ref.h"
#include "Reporter.h"
#include "Sink.h"


/* ================================================================ constants */


/* =================================================================== macros */


/* =============================================================== data types */

/**
 *  A Sink First-In First-Out buffer.
 *  This buffer can always be written to, it overwrites any
 *  data contained if needed.
 *  The class is not thread-safe.
 *
 *  @author  $Author: rafael@riseup.net $
 *  @version $Revision: 474 $
 */
class BufferedSink : public Sink, public virtual Reporter
{
    private:

        /**
         *  The buffer.
         */
        unsigned char     * buffer;

        /**
         *  The end of the buffer.
         */
        unsigned char     * bufferEnd;

        /**
         *  The size of the buffer.
         */
        unsigned int        bufferSize;

        /**
         *  The highest usage of the buffer.
         */
        unsigned int        peak;
        
        /**
         *  All data written to this BufferedSink is handled by chuncks
         *  of this size.
         */
        unsigned int        chunkSize;

        /**
         *  Number of bytes the underlying stream is misaligned with
         *  chunkSize. (It needs this many bytes more to be aligned.)
         */
        unsigned int        misalignment;

        /**
         *  Start of free territory in buffer.
         */
        unsigned char     * inp;

        /**
         *  Start of sensible data in buffer.
         */
        unsigned char     * outp;


        /**
         *  The underlying Sink.
         */
        Ref<Sink>           sink;

        /**
         *  Initialize the object.
         *
         *  @param sink the Sink to attach this BufferedSink to.
         *  @param size the size of the internal buffer to use.
         *  @param chunkSize size of chunks to handle data in.
         *  @exception Exception
         */
        void
        init (  Sink              * sink,
                unsigned int        size,
                unsigned int        chunkSize )         throw ( Exception );

        /**
         *  De-initialize the object.
         *
         *  @exception Exception
         */
        void
        strip ( void )                                  throw ( Exception );

        /**
         *  Slide a pointer in the internal buffer by offset. If the pointer
         *  would reach beyond the end of the buffer, it goes wraps around.
         *
         *  @param p the pointer to slide.
         *  @param offset the amount to slide with.
         *  @return pointer p + offset, wrapped around if needed.
         */
        inline unsigned char *
        slidePointer (
                        unsigned char * p,
                        unsigned int    offset )        throw ()
        {
            p += offset;
            while ( p >= bufferEnd ) {
                p -= bufferSize;
            }

            return p;
        }

        /**
         *  Update the peak buffer usage indicator.
         *
         *  @see #peak
         */
        inline void
        updatePeak ( void )                             throw ()
        {
            unsigned int    u;

            u = outp <= inp ? inp - outp : (bufferEnd - outp) + (inp - buffer);
            if ( peak < u ) {
                peak = u;
                reportEvent( 4, "BufferedSink, new peak:", peak);
                reportEvent( 4, "BufferedSink, remaining:", bufferSize - peak);
            }
        }

        /**
         *  If the underlying Sink is misaligned on chunkSize, write as
         *  many 0s as needed to get it aligned.
         *
         *  @see #misalignment
         *  @see #chunkSize
         */
        inline bool
        align ( void )                                      throw ( Exception )
        {
            char    b[] = { 0 };

            while ( misalignment ) {
                if ( sink->canWrite( 0, 0) ) {
                    unsigned int    ret;
                    
                    if ( !(ret = sink->write( b, 1)) ) {
                        return false;
                    }
                    --misalignment;

                } else {
                    return false;
                }
            }

            return true;
        }


    protected:

        /**
         *  Default constructor. Always throws an Exception.
         *  
         *  @exception Exception
         */
        inline
        BufferedSink ( void )                       throw ( Exception )
        {
            throw Exception( __FILE__, __LINE__);
        }

        /**
         *  Get the size of the buffer.
         *  
         *  @return the size of the buffer.
         */
        inline unsigned int
        getSize ( void ) const                      throw ()
        {
            return bufferSize;
        }

        /**
         *  Store data in the internal buffer. If there is not enough space,
         *  discard all in the buffer and the beginning of the supplied
         *  buffer if needed.
         *  
         *  @param buffer the data to store.
         *  @param bufferSize the amount of data to store in bytes.
         *  @return number of bytes really stored.
         */
        unsigned int
        store (     const void    * buffer,
                    unsigned int    bufferSize )    throw ( Exception );


    public:

        /**
         *  Constructor by an underlying Sink, buffer size and chunk size.
         *  
         *  @param sink the Sink to attach this BufferSink to.
         *  @param size the size of the buffer to use for buffering.
         *  @param chunkSize hanlde all data in write() as chunks of
         *                   chunkSize
         *  @exception Exception
         */
        inline 
        BufferedSink (  Sink              * sink,
                        unsigned int        size,
                        unsigned int        chunkSize = 1 ) throw ( Exception )
        {
            init( sink, size, chunkSize);
        }

        /**
         *  Copy constructor.
         *  
         *  @param buffer the object to copy.
         *  @exception Exception
         */
        BufferedSink (  const BufferedSink &  buffer )  throw ( Exception );

        /**
         *  Destructor.
         *  
         *  @exception Exception
         */
        inline virtual
        ~BufferedSink ( void )                          throw ( Exception )
        {
            strip();
        }

        /**
         *  Assignment operator.
         *  
         *  @param bs the object to assign to this one.
         *  @return a reference to this object.
         *  @exception Exception
         */
        virtual BufferedSink &
        operator= ( const BufferedSink &    bs )        throw ( Exception );

        /**
         *  Get the peak usage of the internal buffer.
         *  
         *  @return the peak usage of the internal buffer.
         */
        inline unsigned int
        getPeak ( void ) const                          throw ()
        {
            return peak;
        }

        /**
         *  Open the BufferedSink. Opens the underlying Sink.
         *  
         *  @return true if opening was successful, false otherwise.
         *  @exception Exception
         */
        inline virtual bool
        open ( void )                                   throw ( Exception )
        {
            return sink->open();
        }

        /**
         *  Check if a BufferedSink is open.
         *
         *  @return true if the BufferedSink is open, false otherwise.
         */
        inline virtual bool
        isOpen ( void ) const                           throw ()
        {
            return sink->isOpen();
        }

        /**
         *  Check if the BufferedSink is ready to accept data.
         *  Always returns true immediately.
         *
         *  @param sec the maximum seconds to block.
         *  @param usec micro seconds to block after the full seconds.
         *  @return true
         *  @exception Exception
         */
        inline virtual bool
        canWrite (     unsigned int    sec,
                       unsigned int    usec )           throw ( Exception )
        {
            return true;
        }

        /**
         *  Write data to the BufferedSink.
         *  Always reads the maximum number of chunkSize chunks buf
         *  holds. If the data can not be written to the underlying
         *  stream, it is buffered. If the buffer overflows, the oldest
         *  data is discarded.
         *
         *  @param buf the data to write.
         *  @param len number of bytes to write from buf.
         *  @return the number of bytes written (may be less than len).
         *  @exception Exception
         */
        virtual unsigned int
        write (    const void    * buf,
                   unsigned int    len )                throw ( Exception );

        /**
         *  Flush all data that was written to the BufferedSink to the
         *  underlying Sink.
         *
         *  @exception Exception
         */
        inline virtual void
        flush ( void )                                  throw ( Exception )
        {
            unsigned char   b[1];

            write( b, 0);
        }

        /**
         *  Cut what the sink has been doing so far, and start anew.
         *  This usually means separating the data sent to the sink up
         *  until now, and start saving a new chunk of data.
         */
        inline virtual void
        cut ( void )                                    throw ()
        {
            flush();
            sink->cut();
        }

        /**
         *  Close the BufferedSink. Closes the underlying Sink.
         *
         *  @exception Exception
         */
        virtual void
        close ( void )                                  throw ( Exception );
};


/* ================================================= external data structures */


/* ====================================================== function prototypes */



#endif  /* BUFFERED_SINK_H */

