/*------------------------------------------------------------------------------

   Copyright (c) 2000-2007 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : Util.h
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
#ifndef UTIL_H
#define UTIL_H

#ifndef __cplusplus
#error This is a C++ include file
#endif


/* ============================================================ include files */

#include "Exception.h"


/* ================================================================ constants */


/* =================================================================== macros */


/* =============================================================== data types */

/**
 *  Widely used utilities.
 *  This class can not be instantiated, but contains useful (?) static
 *  functions.
 *
 *  Typical usage:
 *
 *  <pre>
 *  #include "Util.h"
 *  
 *  char  * str = Util::strDup( otherStr);
 *  </pre>
 *
 *  @author  $Author: rafael@riseup.net $
 *  @version $Revision: 474 $
 */
class Util
{
    private:

        /**
         *  Helper table for base64 encoding.
         */
        static char base64Table[];

    protected:

        /**
         *  Default constructor. Always throws an Exception.
         *
         *  @exception Exception
         */
        inline
        Util ( void )                           throw ( Exception )
        {
            throw Exception( __FILE__, __LINE__);
        }

        /**
         *  Copy constructor. Always throws an Exception.
         *
         *  @exception Exception
         */
        inline
        Util ( const Util &   e )               throw ( Exception )
        {
            throw Exception( __FILE__, __LINE__);
        }

        /**
         *  Destructor. Always throws an Exception.
         *
         *  @exception Exception
         */
        inline
        ~Util ( void )                          throw ( Exception )
        {
            throw Exception( __FILE__, __LINE__);
        }

        /**
         *  Assignment operator. Always throws an Exception.
         *
         *  @param u the object to assign to this one.
         *  @exception Exception
         */
        inline Util &
        operator= ( const Util &   u )          throw ( Exception )
        {
            throw Exception( __FILE__, __LINE__);
        }



    public:

        /**
         *  Determine a C string's length.
         *
         *  @param str a zero-terminated C string.
         *  @return length of str
         *  @exception Exception
         */
        static unsigned int
        strLen (        const char    * str )       throw ( Exception );

        /**
         *  Copy a C string into another.
         *
         *  @param dest place for the copy. Storage size must be at least
         *              Util::strLen(src) + 1 long.
         *  @param src the string to copy.
         *  @exception Exception
         */
        static void
        strCpy (    char          * dest,
                    const char    * src )           throw ( Exception );

        /**
         *  Concatenate a string to another's end.
         *
         *  @param dest the string to concatenate to.
         *              Storage size of dest must be at least
         *              Util::strLen(dest) + Util::strLen(src) + 1 long.
         *  @param src the string to concatenate.
         *  @exception Exception
         */
        static void
        strCat (    char          * dest,
                    const char    * src )           throw ( Exception );

        /**
         *  Duplicate a string by allocating space with new[].
         *  The returned string must be freed with delete[].
         *
         *  @param str the string to duplicate.
         *  @exception Exception
         */
        static char *
        strDup (        const char    * str )       throw ( Exception );

        /**
         *  Determine wether two string are equal.
         *
         *  @param str1 one of the strings.
         *  @param str2 the other string.
         *  @param len check the first most len characters. if 0, check
         *             the whole string
         *  @return true if the two strings are equal, false othersize.
         *  @exception Exception
         */
        static bool
        strEq ( const char    * str1,
                const char    * str2,
                unsigned int    len = 0 )           throw ( Exception );

        /**
         *  Convert a string to long.
         *
         *  @param str the string to convert.
         *  @param base numeric base of number in str.
         *  @return the value of str as a long int
         *  @exception Exception
         */
        static long int
        strToL ( const char    * str,
                 int             base = 10 )         throw ( Exception );

        /**
         *  Convert a string to double.
         *
         *  @param str the string to convert.
         *  @return the value of str as a double
         *  @exception Exception
         */
        static double
        strToD ( const char    * str )              throw ( Exception );

        /**
         *  Add current date to a file name, before the file extension (if any)
         *
         *  @param str the string to convert (file name).
         *  @return the new string with the date appended before 
         *          extension of the file name. the string has to be
         *          deleted with delete[] after it is not needed
         *  @exception Exception
         */
        static char *
        fileAddDate ( const char * str,
                      const char * format = "[%m-%d-%Y-%H-%M-%S]" )
                                                        throw ( Exception );

        /**
         *  Convert a string into base64 encoding.
         *  base64 is described in RFC 2045, section 6.8
         *  The returned string must be freed with delete[].
         *
         *  @param str the string to convert.
         *  @return the supplied string in base64 encoding.
         *  @exception Exception
         */
        static char *
        base64Encode ( const char     * str )       throw ( Exception );

        /**
         *  Convert an unsigned char buffer holding 8 or 16 bit PCM values
         *  with channels interleaved to a short int buffer, still
         *  with channels interleaved.
         *
         *  @param bitsPerSample the number of bits per sample in the input
         *  @param pcmBuffer the input buffer
         *  @param lenPcmBuffer the number of samples total in pcmBuffer
         *                      (e.g. if 2 channel input, this is twice the
         *                       number of sound samples)
         *  @param outBuffer the output buffer, must be big enough
         *  @param isBigEndian true if the input is big endian, false otherwise
         */
        static void
        conv (  unsigned int        bitsPerSample,
                unsigned char     * pcmBuffer,
                unsigned int        lenPcmBuffer,
                short int         * outBuffer,
                bool                isBigEndian = true )    throw ( Exception );


        /**
         *  Convert a short buffer holding PCM values with channels interleaved
         *  to one or more float buffers, one for each channel
         *
         *  @param shortBuffer the input buffer
         *  @param lenShortBuffer total length of the input buffer
         *  @param floatBuffers an array of float buffers, each
         *                      (lenShortBuffer / channels) long
         *  @param channels number of channels to separate the input to
         */
        static void
        conv (  short int         * shortBuffer,
                unsigned int        lenShortBuffer,
                float            ** floatBuffers,
                unsigned int        channels )              throw ( Exception );

        /**
         *  Convert a char buffer holding 8 bit PCM values to a short buffer
         *
         *  @param pcmBuffer buffer holding 8 bit PCM audio values,
         *                   channels are interleaved
         *  @param lenPcmBuffer length of pcmBuffer
         *  @param leftBuffer put the left channel here (must be big enough)
         *  @param rightBuffer put the right channel here (not touched if mono,
         *                     must be big enough)
         *  @param channels number of channels (1 = mono, 2 = stereo)
         */
        static void
        conv8 (     unsigned char     * pcmBuffer,
                    unsigned int        lenPcmBuffer,
                    short int         * leftBuffer,
                    short int         * rightBuffer,
                    unsigned int        channels )          throw ( Exception );

        /**
         *  Convert a char buffer holding 16 bit PCM values to a short buffer
         *
         *  @param pcmBuffer buffer holding 16 bit PCM audio values,
         *                   channels are interleaved
         *  @param lenPcmBuffer length of pcmBuffer
         *  @param leftBuffer put the left channel here (must be big enough)
         *  @param rightBuffer put the right channel here (not touched if mono,
         *                     must be big enough)
         *  @param channels number of channels (1 = mono, 2 = stereo)
         *  @param isBigEndian true if input is big endian, false otherwise
         */
        static void
        conv16 (    unsigned char     * pcmBuffer,
                    unsigned int        lenPcmBuffer,
                    short int         * leftBuffer,
                    short int         * rightBuffer,
                    unsigned int        channels,
                    bool                isBigEndian )       throw ( Exception );

        /**
         *  Make a thread sleep for specified amount of time.
         *  Only the thread which this is called in will sleep.
         *  The SIGUSR1 signal will be blocked during the sleep.
         *
         *  @param sec the number of seconds to sleep.
         *  @param nsec the number of nano-seconds to sleep.
         */
        static void
        sleep(  long    sec,
                long    nsec);
                
};


/* ================================================= external data structures */


/* ====================================================== function prototypes */



#endif  /* UTIL_H */

