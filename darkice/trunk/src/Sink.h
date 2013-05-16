/*------------------------------------------------------------------------------

   Copyright (c) 2000-2007 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : Sink.h
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
#ifndef SINK_H
#define SINK_H

#ifndef __cplusplus
#error This is a C++ include file
#endif


/* ============================================================ include files */

#include "Referable.h"
#include "Exception.h"


/* ================================================================ constants */


/* =================================================================== macros */


/* =============================================================== data types */

/**
 *  A general data sink
 *
 *  @author  $Author: rafael@riseup.net $
 *  @version $Revision: 474 $
 */
class Sink : public virtual Referable
{
    private:


    protected:

        /**
         *  Default constructor.
         */
        inline
        Sink ( void )                                   throw ()
        {
        }

        /**
         *  Copy constructor.
         *
         *  @param sink the Sink to copy.
         */
        inline
        Sink (      const Sink &    sink )              throw ()
        {
        }

        /**
         *  Assignment operator.
         *
         *  @param sink the Sink to assign this to.
         *  @return a reference to this Sink.
         *  @exception Exception
         */
        inline virtual Sink &
        operator= ( const Sink &    sink )              throw ( Exception )
        {
            return *this;
        }


    public:

        /**
         *  Destructor.
         *
         *  @exception Exception
         */
        inline virtual
        ~Sink ( void )                                  throw ( Exception )
        {
        }

        /**
         *  Open the sink.
         *
         *  @return true if opening was successfull, false otherwise.
         *  @exception Exception
         */
        virtual bool
        open ( void )                               throw ( Exception )    = 0;

        /**
         *  Check if the Sink is open.
         *
         *  @return true if the Sink is open, false otherwise.
         */
        virtual bool
        isOpen ( void ) const                       throw ()    = 0;

        /**
         *  Check if the Sink is ready to accept data.
         *  Blocks until the specified time for data to be available.
         *
         *  @param sec the maximum seconds to block.
         *  @param usec micro seconds to block after the full seconds.
         *  @return true if the Sink is ready to accept data, false otherwise.
         *  @exception Exception
         */
        virtual bool
        canWrite (              unsigned int    sec,
                                unsigned int    usec )  throw ( Exception ) = 0;

        /**
         *  Write data to the Sink.
         *
         *  @param buf the data to write.
         *  @param len number of bytes to write from buf.
         *  @return the number of bytes written (may be less than len).
         *  @exception Exception
         */
        virtual unsigned int
        write (                 const void    * buf,
                                unsigned int    len )   throw ( Exception ) = 0;

        /**
         *  Flush all data that was written to the Sink to the underlying
         *  construct.
         *
         *  @exception Exception
         */
        virtual void
        flush ( void )                                  throw ( Exception ) = 0;

        /**
         *  Cut what the sink has been doing so far, and start anew.
         *  This usually means separating the data sent to the sink up
         *  until now, and start saving a new chunk of data.
         */
        virtual void
        cut ( void )                                    throw () = 0;

        /**
         *  Close the Sink.
         *
         *  @exception Exception
         */
        virtual void
        close ( void )                                  throw ( Exception ) = 0;
};


/* ================================================= external data structures */


/* ====================================================== function prototypes */



#endif  /* SINK_H */

