/*------------------------------------------------------------------------------

   Copyright (c) 2000 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : Connector.h
   Version  : $Revision$
   Author   : $Author$
   Location : $Source$
   
   Abstract : 

     Connects a source to a sink

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
#ifndef CONNECTOR_H
#define CONNECTOR_H

#ifndef __cplusplus
#error This is a C++ include file
#endif


/* ============================================================ include files */

#include "Referable.h"
#include "Ref.h"
#include "Source.h"
#include "Sink.h"


/* ================================================================ constants */


/* =================================================================== macros */


/* =============================================================== data types */

/*------------------------------------------------------------------------------
 *  
 *----------------------------------------------------------------------------*/
class Connector : public virtual Referable
{
    private:

        Ref<Source>     source;


        Ref<Sink>     * sinks;
        unsigned int    numSinks;


        void
        init (  Source            * source )        throw ( Exception );


        void
        strip ( void )                              throw ( Exception );


    protected:

        inline
        Connector ( void )                          throw ( Exception )
        {
            throw Exception( __FILE__, __LINE__);
        }


    public:

        inline
        Connector (     Source        * source )    throw ( Exception )
        {
            init( source);
        }


        inline
        Connector ( Source            * source,
                    Sink              * sink )        throw ( Exception )
        {
            init( source);
            attach( sink);
        }

        
        Connector ( const Connector &   connector )     throw ( Exception );
        

        inline virtual
        ~Connector( void )
        {
            strip();
        }


        virtual Connector &
        operator= ( const Connector &   connector )     throw ( Exception );


        inline unsigned int
        getNumSinks ( void ) const              throw ()
        {
            return numSinks;
        }


        void
        attach (    Sink          * sink )              throw ( Exception );


        bool
        detach (    Sink          * sink )          throw ( Exception );


        bool
        open ( void )                                   throw ( Exception );


        unsigned int
        transfer (  unsigned int        bytes,
                    unsigned int        bufSize,
                    unsigned int        sec,
                    unsigned int        usec )          throw ( Exception );


        void
        close ( void )                                  throw ( Exception );
};


/* ================================================= external data structures */


/* ====================================================== function prototypes */



#endif  /* CONNECTOR_H */


/*------------------------------------------------------------------------------
 
  $Source$

  $Log$
  Revision 1.1  2000/11/05 10:05:49  darkeye
  Initial revision

  
------------------------------------------------------------------------------*/

