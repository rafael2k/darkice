/*------------------------------------------------------------------------------

   Copyright (c) 2000-2007 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : Connector.h
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
#ifndef CONNECTOR_H
#define CONNECTOR_H

#ifndef __cplusplus
#error This is a C++ include file
#endif


/* ============================================================ include files */

#include "Referable.h"
#include "Ref.h"
#include "Reporter.h"
#include "Source.h"
#include "Sink.h"


/* ================================================================ constants */


/* =================================================================== macros */


/* =============================================================== data types */

/**
 *  Connects a source to one or more sinks.
 *
 *  @author  $Author$
 *  @version $Revision$
 */
class Connector : public virtual Referable, public virtual Reporter
{
    private:

        /**
         *  Initialize the object.
         *
         *  @param source the source to read from.
         *  @exception Exception
         */
        void
        init (  Source            * source )        throw ( Exception );

        /**
         *  De-initialize the object.
         *
         *  @exception Exception
         */
        void
        strip ( void )                              throw ( Exception );


    protected:

        /**
         *  The source to read from.
         */
        Ref<Source>     source;

        /**
         *  The sinks to connect the source to.
         */
        Ref<Sink>     * sinks;

        /**
         *  Total number of sinks.
         */
        unsigned int    numSinks;

        /**
         *  Default constructor. Always throws an Exception.
         *
         *  @exception Exception
         */
        inline
        Connector ( void )                          throw ( Exception )
        {
            throw Exception( __FILE__, __LINE__);
        }

        /**
         *  Detach an already attached Sink from the Source of this Connector.
         *
         *  @param sink the Sink to detach.
         *  @return true if the detachment was successful, false otherwise.
         *  @exception Exception
         */
        virtual bool
        detach (    Sink          * sink )          throw ( Exception );


    public:

        /**
         *  Constructor based on a Source.
         *
         *  @param source the source to connect to the sinks.
         *  @exception Exception
         */
        inline
        Connector (     Source        * source )    throw ( Exception )
        {
            init( source);
        }

        /**
         *  Constructor based on a Source and a Sink.
         *
         *  @param source the source to connect to the sinks.
         *  @param sink a sink to connect to the source.
         *  @exception Exception
         */
        inline
        Connector ( Source            * source,
                    Sink              * sink )        throw ( Exception )
        {
            init( source);
            attach( sink);
        }

        /**
         *  Copy constructor.
         *
         *  @param connector the object to copy.
         *  @exception Exception
         */
        Connector ( const Connector &   connector )     throw ( Exception );
        
        /**
         *  Destructor.
         *
         *  @exception Exception
         */
        inline virtual
        ~Connector( void )                              throw ( Exception )
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
        virtual Connector &
        operator= ( const Connector &   connector )     throw ( Exception );

        /**
         *  Get the number of Sinks in the Connector.
         *
         *  @return the number of Sinks in the Connector.
         *  @exception Exception
         */
        inline virtual unsigned int
        getNumSinks ( void ) const              throw ()
        {
            return numSinks;
        }

        /**
         *  Attach a Sink to the Source of this Connector.
         *
         *  @param sink the Sink to attach.
         *  @exception Exception
         */
        virtual void
        attach (    Sink          * sink )              throw ( Exception );

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
         *  @param usec the number of micro seconds to wait for the Source to
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
};


/* ================================================= external data structures */


/* ====================================================== function prototypes */



#endif  /* CONNECTOR_H */

