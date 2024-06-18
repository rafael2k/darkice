/*------------------------------------------------------------------------------

   Copyright (c) 2000-2007 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : Util.cpp
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

/* ============================================================ include files */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_ERRNO_H
#include <errno.h>
#else
#error need errno.h
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#else
#error need string.h
#endif

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#else
#error need stdlib.h
#endif

#ifdef HAVE_LIMITS_H
#include <limits.h>
#else
#error need limits.h
#endif

#ifdef HAVE_MATH_H
#include <math.h>
#else
#error need math.h
#endif

#ifdef HAVE_TIME_H
#include <time.h>
#else
#error need time.h
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#else
#error need unistd.h
#endif

#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#else
#error need sys/time.h
#endif

#ifdef HAVE_SIGNAL_H
#include <signal.h>
#else
#error need signal.h
#endif

#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif

#include "Util.h"


/* ===================================================  local data structures */


/* ================================================  local constants & macros */

/*------------------------------------------------------------------------------
 *  File identity
 *----------------------------------------------------------------------------*/
static const char fileid[] = "$Id$";


/* ===============================================  local function prototypes */


/* =============================================================  module code */

char
Util :: base64Table[] = {
    'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P',
    'Q','R','S','T','U','V','W','X','Y','Z','a','b','c','d','e','f',
    'g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v',
    'w','x','y','z','0','1','2','3','4','5','6','7','8','9','+','/'
};


/*------------------------------------------------------------------------------
 *  Calculate the length of a zero-terminated C string,
 *  w/o the zero-termination
 *----------------------------------------------------------------------------*/
unsigned int
Util :: strLen( const char    * str )                   
{
    size_t      len;

    if ( !str ) {
        throw Exception( __FILE__, __LINE__, "no str");
    }

    len = strlen( str);

    return len;
}


/*------------------------------------------------------------------------------
 *  Copy the contents of a string into another
 *----------------------------------------------------------------------------*/
void
Util :: strCpy (    char          * dest,
                    const char    * src )               
{
    if ( !dest || !src ) {
        throw Exception( __FILE__, __LINE__, "no src or dest");
    }

    strcpy( dest, src);
}


/*------------------------------------------------------------------------------
 *  Concatenate the contents of a string onto another
 *----------------------------------------------------------------------------*/
void
Util :: strCat (    char          * dest,
                    const char    * src )               
{
    if ( !dest || !src ) {
        throw Exception( __FILE__, __LINE__, "no src or dest");
    }

    strcat( dest, src);
}


/*------------------------------------------------------------------------------
 *  Duplicate a string by allocating space with new[]
 *  The returned string must be freed with delete[]
 *----------------------------------------------------------------------------*/
char *
Util :: strDup( const char    * str )                   
{
    size_t      len;
    char      * s;

    if ( !str ) {
        throw Exception( __FILE__, __LINE__, "no str");
    }

    len = strlen( str) + 1;
    s   = new char[len];
    memcpy( s, str, len);

    return s;
}


/*------------------------------------------------------------------------------
 *  Convert a string into base64 encoding.
 *----------------------------------------------------------------------------*/
char *
Util :: base64Encode( const char  * str )               
{
    if ( !str ) {
        throw Exception( __FILE__, __LINE__, "no str");
    }

    const char    * data    = str;
    size_t          len     = strlen( data);
    char          * out     = new char[len * 4 / 3 + 4];
    char          * result  = out;
    unsigned        chunk;

    while ( len > 0 ) {
        chunk = (len > 3) ? 3 : len;
        *out++ = base64Table[(*data & 0xfc) >> 2];
        *out++ = base64Table[((*data & 0x03) << 4) | ((*(data+1) & 0xf0) >> 4)];
        switch ( chunk ) {
            case 3:
                *out++ = base64Table[((*(data+1) & 0x0f) << 2) |
                                     ((*(data+2) & 0xc0) >> 6)];
                *out++ = base64Table[(*(data+2)) & 0x3f];
                break;
            case 2:
                *out++ = base64Table[((*(data+1) & 0x0f) << 2)];
                *out++ = '=';
                break;
            case 1:
                *out++ = '=';
                *out++ = '=';
                break;
        }
        data += chunk;
        len  -= chunk;
    }
    *out = 0;

    return result;
}


/*------------------------------------------------------------------------------
 *  Check whether two strings are equal
 *----------------------------------------------------------------------------*/
bool
Util :: strEq( const char    * str1,
               const char    * str2,
               unsigned int    len )                    
{
    if ( !str1 || !str2 ) {
        throw Exception( __FILE__, __LINE__, "no str1 or no str2");
    }

    return len == 0 ? !strcmp( str1, str2) : !strncmp( str1, str2, len);
}


/*------------------------------------------------------------------------------
 *  Convert a string to a long integer
 *----------------------------------------------------------------------------*/
long int
Util :: strToL( const char *str) 
{
    long int   val;
    char      *end;

    if ( NULL == str ) 
        throw Exception( __FILE__, __LINE__, "null pointer parameter, not string");
    
    errno = 0; // set it, strtol() can change it 
    val = strtol( str, &end, 10); 
    
    if (end == str)
        throw Exception( __FILE__, __LINE__, "number conversion error, not a decimal string");
    
    if ((LONG_MIN == val || LONG_MAX ==  val) && ERANGE == errno) 
        throw Exception( __FILE__, __LINE__, "number conversion error, out of range of type long");
    
    return val;
}


/*------------------------------------------------------------------------------
 *  Convert a string to a double
 *----------------------------------------------------------------------------*/
double
Util :: strToD( const char    * str )                   
{
    double      val;
    char      * s;

    if ( !str ) {
        throw Exception( __FILE__, __LINE__, "no str");
    }

    val = strtod( str, &s);
    if ( s == str || errno == ERANGE ) {
        throw Exception( __FILE__, __LINE__, "number conversion error");
    }

    return val;
}


/*------------------------------------------------------------------------------
 *  add current date to a file name, before the file extension (if any)
 *----------------------------------------------------------------------------*/
char *
Util :: fileAddDate ( const char * str,
                      const char * format )             
{
    unsigned int    size;
    const char          * last; 
    char          * s;
    char          * strdate;
    time_t          now;

    if ( !str ) {
        throw Exception( __FILE__, __LINE__, "no str");
    }

    strdate = new char[128];
    now     = time(NULL);    
    strftime( strdate, 128, format, localtime (&now));

    // search for the part before the extension of the file name
    if ( !(last = strrchr( str, '.')) ) {
        last = (char *) str + strlen( str);
    }

    size = strlen( str) + strlen( strdate) + 1;
    s    = new char [size];

    memcpy( s, str, strlen (str)-strlen(last));
    memcpy( s + strlen(str) -  strlen(last), strdate, strlen (strdate));
    memcpy( s + strlen(str) -  strlen(last) + strlen(strdate),
            last,
            strlen(last));
    s[size-1] = '\0';

    delete[] strdate;   
    return s;
}

/*------------------------------------------------------------------------------
 *  Convert an unsigned char buffer holding 8 or 16 bit PCM values with
 *  channels interleaved to a int16_t buffer, still with channels interleaved
 *----------------------------------------------------------------------------*/
template <typename T> void
Util :: conv (  unsigned int        bitsPerSample,
                unsigned char     * pcmBuffer,
                size_t              lenPcmBuffer,
                T                 * outBuffer,
                bool                isBigEndian )
{
    if ( bitsPerSample == 8 ) {
        unsigned int    i, j;

        for ( i = 0, j = 0; i < lenPcmBuffer; ) {
            outBuffer[j] = pcmBuffer[i++];
            ++j;
        }
    } else if ( bitsPerSample == 16 ) {

        if ( isBigEndian ) {
            unsigned int    i, j;

            for ( i = 0, j = 0; i < lenPcmBuffer; ) {
                int16_t         value;

                value         = pcmBuffer[i++] << 8;
                value        |= pcmBuffer[i++];
                outBuffer[j]  = value;
                ++j;
            }
        } else {
            unsigned int    i, j;

            for ( i = 0, j = 0; i < lenPcmBuffer; ) {
                int16_t         value;

                value         = pcmBuffer[i++];
                value        |= pcmBuffer[i++] << 8;
                outBuffer[j]  = value;
                ++j;
            }
        }
    } else {
        throw Exception( __FILE__, __LINE__,
                         "this number of bits per sample not supported",
                         bitsPerSample);
    }
}
template void Util::conv<int16_t>(unsigned int, unsigned char *, size_t, int16_t *, bool);
template void Util::conv<int32_t>(unsigned int, unsigned char *, size_t, int32_t *, bool);


/*------------------------------------------------------------------------------
 *  Convert a short buffer holding PCM values with channels interleaved
 *  to one or more float buffers, one for each channel
 *----------------------------------------------------------------------------*/
void
Util :: conv (  int16_t           * shortBuffer,
                size_t              lenShortBuffer,
                float            ** floatBuffers,
                unsigned int        channels )
{
    unsigned int    i, j;

    for ( i = 0, j = 0; i < lenShortBuffer; ) {
        for ( unsigned int c = 0; c < channels; ++c ) {
            floatBuffers[c][j] = ((float) shortBuffer[i++]) / 32768.f;
        }
        ++j;
    }
}


/*------------------------------------------------------------------------------
 *  Convert an unsigned char buffer holding 8 bit PCM values with channels
 *  interleaved to two int16_t buffers (one for each channel)
 *----------------------------------------------------------------------------*/
void
Util :: conv8 (     unsigned char     * pcmBuffer,
                    size_t              lenPcmBuffer,
                    int16_t           * leftBuffer,
                    int16_t           * rightBuffer,
                    unsigned int        channels )
{
    if ( channels == 1 ) {
        unsigned int    i, j;

        for ( i = 0, j = 0; i < lenPcmBuffer; ) {
            leftBuffer[j] = pcmBuffer[i++];
            ++j;
        }
    } else if ( channels == 2 ) {
        unsigned int    i, j;

        for ( i = 0, j = 0; i < lenPcmBuffer; ) {
            leftBuffer[j] = pcmBuffer[i++];
            rightBuffer[j] = pcmBuffer[i++];
            ++j;
        }
    } else {
        throw Exception( __FILE__, __LINE__,
                         "this number of channels not supported", channels);
    }
}


/*------------------------------------------------------------------------------
 *  Convert an unsigned char buffer holding 16 bit PCM values with channels
 *  interleaved to two short int buffers (one for each channel)
 *----------------------------------------------------------------------------*/
void
Util :: conv16 (    unsigned char     * pcmBuffer,
                    size_t              lenPcmBuffer,
                    int16_t           * leftBuffer,
                    int16_t           * rightBuffer,
                    unsigned int        channels,
                    bool                isBigEndian )
{
    if ( isBigEndian ) {
        if ( channels == 1 ) {
            unsigned int    i, j;

            for ( i = 0, j = 0; i < lenPcmBuffer; ) {
                int16_t           value;

                value           = pcmBuffer[i++] << 8;
                value          |= pcmBuffer[i++];
                leftBuffer[j]  =  value;
                ++j;
            }
        } else {
            unsigned int    i, j;

            for ( i = 0, j = 0; i < lenPcmBuffer; ) {
                int16_t           value;

                value           = pcmBuffer[i++] << 8;
                value          |= pcmBuffer[i++];
                leftBuffer[j]   = value;
                value           = pcmBuffer[i++] << 8;
                value          |= pcmBuffer[i++];
                rightBuffer[j]  = value;
                ++j;
            }
        }
    } else {
        if ( channels == 1 ) {
            unsigned int    i, j;

            for ( i = 0, j = 0; i < lenPcmBuffer; ) {
                int16_t          value;

                value          = pcmBuffer[i++];
                value         |= pcmBuffer[i++] << 8;
                leftBuffer[j]  = value;
                ++j;
            }
        } else {
            unsigned int    i, j;

            for ( i = 0, j = 0; i < lenPcmBuffer; ) {
                int16_t           value;

                value           = pcmBuffer[i++];
                value          |= pcmBuffer[i++] << 8;
                leftBuffer[j]   = value;
                value           = pcmBuffer[i++];
                value          |= pcmBuffer[i++] << 8;
                rightBuffer[j]  = value;
                ++j;
            }
        }
    }
}


/*------------------------------------------------------------------------------
 *  Make a thread sleep for a specified amount of time.
 *----------------------------------------------------------------------------*/
void
Util :: sleep   (   long            sec,
                    long            nsec)
{
    struct timespec     timespec;
    sigset_t            sigset;

    timespec.tv_sec  = sec;
    timespec.tv_nsec = nsec;

    // mask out SIGUSR1, as we're expecting that signal for other reasons
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGUSR1);

    pselect( 0, NULL, NULL, NULL, &timespec, &sigset);
}
