/*------------------------------------------------------------------------------

   Copyright (c) 2000 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : LameEncoder.cpp
   Version  : $Revision$
   Author   : $Author$
   Location : $Source$
   
   Abstract : 

     A class representing the lame mp3 encoder

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

/* ============================================================ include files */

#include <stdio.h>

#include "Exception.h"
#include "LameEncoder.h"


/* ===================================================  local data structures */


/* ================================================  local constants & macros */

/*------------------------------------------------------------------------------
 *  File identity
 *----------------------------------------------------------------------------*/
static const char fileid[] = "$Id$";


/*------------------------------------------------------------------------------
 *  File identity
 *----------------------------------------------------------------------------*/
#define ARG_LEN     64


/* ===============================================  local function prototypes */


/* =============================================================  module code */

/*------------------------------------------------------------------------------
 *  Initialize command line parameters
 *----------------------------------------------------------------------------*/
unsigned int
LameEncoder :: makeArgs ( void )            throw ( Exception )
{
    unsigned int        u;
    char                str[ARG_LEN];

    u = 0;

    setArg( getEncoderName(),   u++);           // name of the command
    setArg( "-S",               u++);           // make it silent
    setArg( "-r",               u++);           // input is raw PCM
    setArg( "-x",               u++);           // input is little endian
    setArg( "-h",               u++);           // high quality

    /* set input sample rate */
    if ( snprintf( str, ARG_LEN, "%.3f",
                   ((double)getInSampleRate()) / 1000.0 ) == -1 ) {
        throw Exception( __FILE__, __LINE__, "snprintf overflow");
    }
    setArg( "-s",               u++);
    setArg( str,                u++);

    /* set stereo / mono */
    setArg( "-m",               u++);
    setArg( getOutChannel() == 1 ? "m" : "j",   u++);

    /* set output bitrate */
    if ( snprintf( str, ARG_LEN, "%d", getOutBitrate()) == -1 ) {
        throw Exception( __FILE__, __LINE__, "snprintf overflow");
    }
    setArg( "-b",               u++);
    setArg( str,                u++);

    /* set output sample rate */
    if ( snprintf( str, ARG_LEN, "%.3f",
                   ((double)getOutSampleRate()) / 1000.0 ) == -1 ) {
        throw Exception( __FILE__, __LINE__, "snprintf overflow");
    }
    setArg( "--resample",       u++);
    setArg( str,                u++);

    /* set lowpass filter if needed */
    if ( lowpass ) {
        if ( snprintf( str, ARG_LEN, "%.3f",
                       ((double)getLowpass()) / 1000.0 ) == -1 ) {
            throw Exception( __FILE__, __LINE__, "snprintf overflow");
        }
        setArg( "--lowpass",        u++);
        setArg( str,                u++);
    }

    /* set highpass filter if needed */
    if ( highpass ) {
        if ( snprintf( str, ARG_LEN, "%.3f",
                       ((double)getHighpass()) / 1000.0 ) == -1 ) {
            throw Exception( __FILE__, __LINE__, "snprintf overflow");
        }
        setArg( "--highpass",       u++);
        setArg( str,                u++);
    }

    setArg( getInFileName(),    u++);           // input file
    setArg( getOutFileName(),   u++);           // output file

    setArg( 0,                  u++);           // terminating zero


    return u;
}



/*------------------------------------------------------------------------------
 
  $Source$

  $Log$
  Revision 1.1  2000/11/05 10:05:52  darkeye
  Initial revision

  
------------------------------------------------------------------------------*/

