/*------------------------------------------------------------------------------

   Copyright (c) 2000-2007 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : Source.h
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
#ifndef SOURCE_H
#define SOURCE_H

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
 *  A general data source
 *
 *  @author  $Author: rafael@riseup.net $
 *  @version $Revision: 474 $
 */
class Source : public virtual Referable
{
    private:

    protected:

        /**
         *  Default Constructor.
         *
         *  @exception Exception
         */
        inline
        Source ( void )                             throw ( Exception )
        {
        }

        /**
         *  Copy Constructor.
         *
         *  @param source the object to copy.
         *  @exception Exception
         */
        inline
        Source (    const Source &  source )        throw ( Exception )
        {
        }

        /**
         *  Assignment operator.
         *
         *  @param source the object to assign to this one.
         *  @return a reference to this object.
         *  @exception Exception
         */
        inline virtual Source &
        operator= ( const Source &  source )        throw ( Exception )
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
        ~Source ( void )                            throw ( Exception )
        {
        }

        /**
         *  Open the Source.
         *
         *  @return true if opening was successful, false otherwise
         *  @exception Exception
         */
        virtual bool
        open ( void )                           throw ( Exception )     = 0;

        /**
         *  Check if the Source is open.
         *
         *  @return true if the Source is open, false otherwise.
         */
        virtual bool
        isOpen ( void ) const                   throw ()                = 0;

        /**
         *  Check if the Source can be read from.
         *  Blocks until the specified time for data to be available.
         *
         *  @param sec the maximum seconds to block.
         *  @param usec micro seconds to block after the full seconds.
         *  @return true if the Source is ready to be read from,
         *          false otherwise.
         *  @exception Exception
         */
        virtual bool
        canRead (  unsigned int    sec,
                   unsigned int    usec )       throw ( Exception )     = 0;

        /**
         *  Read from the Source.
         *
         *  @param buf the buffer to read into.
         *  @param len the number of bytes to read into buf
         *  @return the number of bytes read (may be less than len).
         *  @exception Exception
         */
        virtual unsigned int
        read (     void          * buf,
                   unsigned int    len )        throw ( Exception )     = 0;

        /**
         *  Close the Source.
         *
         *  @exception Exception
         */
        virtual void
        close ( void )                          throw ( Exception )     = 0;
};


/* ================================================= external data structures */


/* ====================================================== function prototypes */



#endif  /* SOURCE_H */

