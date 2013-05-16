/*------------------------------------------------------------------------------

   Copyright (c) 2000-2007 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : FileCast.h
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
#ifndef FILE_CAST_H
#define FILE_CAST_H

#ifndef __cplusplus
#error This is a C++ include file
#endif


/* ============================================================ include files */

#include "Ref.h"
#include "Sink.h"
#include "CastSink.h"
#include "FileSink.h"
#include "FileCast.h"


/* ================================================================ constants */


/* =================================================================== macros */


/* =============================================================== data types */

/**
 *  Class representing output to a local file.
 *
 *  @author  $Author: rafael@riseup.net $
 *  @version $Revision: 474 $
 */
class FileCast : public CastSink
{
    private:

        /**
         *  The file to send the encoded data to.
         */
        Ref<FileSink>       targetFile;

        /**
         *  Initalize the object.
         *
         *  @param targetFile the file to send the encoded data to.
         *  @exception Exception
         */
        inline void
        init (  FileSink          * targetFile )
                                                    throw ( Exception )
        {
            this->targetFile = targetFile;
        }

        /**
         *  De-initalize the object.
         *
         *  @exception Exception
         */
        inline void
        strip ( void )                              throw ( Exception )
        {
            if ( isOpen() ) {
                close();
            }
        }


    protected:

        /**
         *  Default constructor. Always throws an Exception.
         *
         *  @exception Exception
         */
        inline
        FileCast ( void )                            throw ( Exception )
        {
            throw Exception( __FILE__, __LINE__);
        }

        /**
         *  Log in to the server using the socket avialable.
         *  No need to log in to a file.
         *
         *  @return true if login was successful, false otherwise.
         *  @exception Exception
         */
        inline virtual bool
        sendLogin ( void )              throw ( Exception )
        {
            return true;
        }


    public:

        /**
         *  Constructor.
         *
         *  @param targetFile the file to send all the data to.
         *  @exception Exception
         */
        inline
        FileCast (  FileSink          * targetFile )
                                                        throw ( Exception )
                : CastSink( 0, 0, 0)
        {
            init( targetFile );
        }

        /**
         *  Copy constructor.
         *
         *  @param cs the FileCast to copy.
         */
        inline
        FileCast(   const FileCast &    cs )        throw ( Exception )
        {
            init( targetFile.get() );
        }

        /**
         *  Destructor.
         *
         *  @exception Exception
         */
        inline virtual
        ~FileCast( void )                           throw ( Exception )
        {
            strip();
        }

        /**
         *  Assignment operator.
         *
         *  @param cs the FileCast to assign this to.
         *  @return a reference to this FileCast.
         *  @exception Exception
         */
        inline virtual FileCast &
        operator= ( const FileCast &    cs )        throw ( Exception )
        {
            if ( this != &cs ) {
                strip();
                init( targetFile.get() );
            }
            return *this;
        }

        /**
         *  Open the FileCast.
         *
         *  @return true if opening was successfull, false otherwise.
         *  @exception Exception
         */
        virtual bool
        open ( void )                               throw ( Exception );

        /**
         *  Check if the FileCast is open.
         *
         *  @return true if the FileCast is open, false otherwise.
         */
        inline virtual bool
        isOpen ( void ) const                       throw ()
        {
            return targetFile->isOpen();
        }

        /**
         *  Check if the FileCast is ready to accept data.
         *  Blocks until the specified time for data to be available.
         *
         *  @param sec the maximum seconds to block.
         *  @param usec micro seconds to block after the full seconds.
         *  @return true if the FileCast is ready to accept data,
         *          false otherwise.
         *  @exception Exception
         */
        inline virtual bool
        canWrite (     unsigned int    sec,
                       unsigned int    usec )       throw ( Exception )
        {
            return targetFile->canWrite( sec, usec);
        }

        /**
         *  Write data to the FileCast.
         *
         *  @param buf the data to write.
         *  @param len number of bytes to write from buf.
         *  @return the number of bytes written (may be less than len).
         *  @exception Exception
         */
        inline virtual unsigned int
        write (        const void    * buf,
                       unsigned int    len )        throw ( Exception )
        {
            return targetFile->write( buf, len);
        }

        /**
         *  Flush all data that was written to the FileCast to the server.
         *
         *  @exception Exception
         */
        inline virtual void
        flush ( void )                              throw ( Exception )
        {
            return targetFile->flush();
        }

        /**
         *  Cut what the sink has been doing so far, and start anew.
         *  This usually means separating the data sent to the sink up
         *  until now, and start saving a new chunk of data.
         */
        inline virtual void
        cut ( void )                                    throw ()
        {
            targetFile->cut();
        }

        /**
         *  Close the FileCast.
         *
         *  @exception Exception
         */
        inline virtual void
        close ( void )                              throw ( Exception )
        {
            return targetFile->close();
        }

};


/* ================================================= external data structures */


/* ====================================================== function prototypes */



#endif  /* FILE_CAST_H */

