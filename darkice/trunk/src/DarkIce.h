/*------------------------------------------------------------------------------

   Copyright (c) 2000 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : DarkIce.h
   Version  : $Revision$
   Author   : $Author$
   Location : $Source$
   
   Abstract : 

     Program main object

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
#ifndef DARK_ICE_H
#define DARK_ICE_H

#ifndef __cplusplus
#error This is a C++ include file
#endif


/* ============================================================ include files */

#include <iostream.h>

#include "Referable.h"
#include "Exception.h"
#include "Ref.h"
#include "OssDspSource.h"
#include "PipeSink.h"
#include "BufferedSink.h"
#include "Connector.h"
#include "LameEncoder.h"
#include "PipeSource.h"
#include "TcpSocket.h"
#include "IceCast.h"


/* ================================================================ constants */


/* =================================================================== macros */


/* =============================================================== data types */

/*------------------------------------------------------------------------------
 *  
 *----------------------------------------------------------------------------*/
class DarkIce : public virtual Referable
{
    private:

        Ref<OssDspSource>       dsp;
        Ref<PipeSink>           encInPipe;
        Ref<BufferedSink>       encIn;
        Ref<Connector>          encConnector;
        Ref<LameEncoder>        encoder;

        Ref<PipeSource>         encOutPipe;
        Ref<TcpSocket>          socket;
        Ref<IceCast>            ice;
        Ref<Connector>          shoutConnector;


        void
        init ( void )                               throw ( Exception );


        bool
        encode ( void )                             throw ( Exception );


        bool
        shout ( void )                              throw ( Exception );


    protected:
/*
        virtual void
        showUsage ( ostream       & os )            throw ( Exception );
*/

    public:

        inline
        DarkIce ( void )                            throw ( Exception )
        {
        }


        inline
        DarkIce (   int         argc,
                    char      * argv[] )            throw ()
        {
        }


        inline virtual
        ~DarkIce ( void )                           throw ( Exception )
        {
        }


/*

        inline
        DarkIce ( const DarkIce   & di )            throw ( Exception )
        {
        }


        inline DarkIce &
        operator= ( const DarkIce   * di )          throw ( Exception )
        {
        }
*/

        virtual int
        run ( void )                                throw ( Exception );

};


/* ================================================= external data structures */


/* ====================================================== function prototypes */



#endif  /* DARK_ICE_H */


/*------------------------------------------------------------------------------
 
  $Source$

  $Log$
  Revision 1.1  2000/11/05 10:05:50  darkeye
  Initial revision

  
------------------------------------------------------------------------------*/

