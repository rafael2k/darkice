/*------------------------------------------------------------------------------

   Copyright (c) 2000 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : FileSource.h
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
#ifndef FILE_SOURCE_H
#define FILE_SOURCE_H

#ifndef __cplusplus
#error This is a C++ include file
#endif


/* ============================================================ include files */

#include "Source.h"


/* ================================================================ constants */


/* =================================================================== macros */


/* =============================================================== data types */

/**
 *  A data source based on a file
 *
 *  @author  $Author$
 *  @version $Revision$
 */
class FileSource : public Source
{
    private:

        /**
         *  Name of the file represented by the FileSource.
         */
        char      * fileName;

        /**
         *  Initialize the object.
         *  
         *  @param name name of the file to be represented by the object.
         *  @exception Exception
         */
        void
        init (  const char        * name )              throw ( Exception );

        /**
         *  De-initialize the object.
         *  
         *  @exception Exception
         */
        void
        strip ( void )                                  throw ( Exception );


    protected:

        /**
         *  Low-level file descriptor for the file represented by this object.
         */
        int         fileDescriptor;

        /**
         *  Default constructor. Always throws an Exception.
         *  
         *  @exception Exception
         */
        inline
        FileSource ( void )                             throw ( Exception )
        {
            throw Exception( __FILE__, __LINE__);
        }


    public:

        /**
         *  Constructor by a file name.
         *
         *  @param name name of the file to be represented by the object.
         *  @exception Exception
         */
        inline
        FileSource (    const char        * name )      throw ( Exception )
        {
            init( name);
        }

        /**
         *  Copy constructor.
         *
         *  @param fs the FileSource to copy.
         *  @exception Exception
         */
        FileSource (    const FileSource &  fs )        throw ( Exception );

        /**
         *  Destructor.
         *
         *  @exception Exception
         */
        inline
        ~FileSource ( void )                            throw ( Exception )
        {
            strip();
        }

        /**
         *  Assignment operator.
         *
         *  @param fs the FileSource to assign to this object.
         *  @return a reference to this object.
         *  @exception Exception
         */
        inline virtual FileSource &
        operator= (     const FileSource &  fs )        throw ( Exception );

        /**
         *  Get the file name this FileSource represents.
         *
         *  @return the file name this FileSource represents.
         */
        inline virtual const char *
        getFileName ( void ) const                      throw ()
        {
            return fileName;
        }

        /**
         *  Check for the existence of the file this FileSource represents.
         *
         *  @return true if the file exists, false otherwise.
         */
        virtual bool
        exists ( void ) const                           throw ();

        /**
         *  Open the file.
         *
         *  @return true if opening was successful, false otherwise.
         *  @exception Exception
         */
        virtual bool
        open ( void )                                   throw ( Exception );

        /**
         *  Check if the FileSource is open.
         *
         *  @return true if the FileSource is open, false otherwise.
         */
        inline virtual bool
        isOpen ( void ) const                           throw ()
        {
            return fileDescriptor != 0;
        }

        /**
         *  Check if the FileSource has data available.
         *  Blocks until the specified time for data to be available.
         *
         *  @param sec the maximum seconds to block.
         *  @param usec micro seconds to block after the full seconds.
         *  @return true if the FileSource has data to be read,
         *          false otherwise.
         *  @exception Exception
         */
        virtual bool
        canRead (               unsigned int    sec,
                                unsigned int    usec )  throw ( Exception );

        /**
         *  Read from the FileSource.
         *
         *  @param buf the buffer to read into.
         *  @param len the number of bytes to read into buf
         *  @return the number of bytes read (may be less than len).
         *  @exception Exception
         */
        virtual unsigned int
        read (                  void          * buf,
                                unsigned int    len )   throw ( Exception );

        /**
         *  Close the FileSource.
         *
         *  @exception Exception
         */
        virtual void
        close ( void )                                  throw ( Exception );
};


/* ================================================= external data structures */


/* ====================================================== function prototypes */



#endif  /* FILE_SOURCE_H */


/*------------------------------------------------------------------------------
 
  $Source$

  $Log$
  Revision 1.2  2000/11/12 13:31:40  darkeye
  added kdoc-style documentation comments

  Revision 1.1.1.1  2000/11/05 10:05:51  darkeye
  initial version

  
------------------------------------------------------------------------------*/

