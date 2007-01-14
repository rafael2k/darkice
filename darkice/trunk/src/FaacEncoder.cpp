/*------------------------------------------------------------------------------

   Copyright (c) 2000-2007 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : FaacEncoder.cpp
   Version  : $Revision$
   Author   : $Author$
   Location : $HeadURL$
   
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

/* ============================================================ include files */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

// compile the whole file only if faac support configured in
#ifdef HAVE_FAAC_LIB



#include "Exception.h"
#include "Util.h"
#include "FaacEncoder.h"


/* ===================================================  local data structures */


/* ================================================  local constants & macros */

/*------------------------------------------------------------------------------
 *  File identity
 *----------------------------------------------------------------------------*/
static const char fileid[] = "$Id$";


/* ===============================================  local function prototypes */


/* =============================================================  module code */

/*------------------------------------------------------------------------------
 *  Open an encoding session
 *----------------------------------------------------------------------------*/
bool
FaacEncoder :: open ( void )
                                                            throw ( Exception )
{
    if ( isOpen() ) {
        close();
    }

    // open the underlying sink
    if ( !getSink()->open() ) {
        throw Exception( __FILE__, __LINE__,
                         "faac lib opening underlying sink error");
    }

    char      * faacVersion;
    char      * faacCopyright;
    faacEncGetVersion(&faacVersion, &faacCopyright);
    reportEvent(1, "Using faac codec version", faacVersion);

    encoderHandle = faacEncOpen(getInSampleRate(),
                                getInChannel(),
                                &inputSamples,
                                &maxOutputBytes);

    faacEncConfiguration      * faacConfig;

    faacConfig = faacEncGetCurrentConfiguration(encoderHandle);

    faacConfig->aacObjectType = MAIN;
    faacConfig->mpegVersion   = MPEG2;
    faacConfig->useTns        = 1;
    faacConfig->shortctl      = SHORTCTL_NORMAL;
    faacConfig->useLfe        = 0;
    faacConfig->allowMidside  = 1;
    faacConfig->bitRate       = getOutBitrate() * 1000 / getOutChannel();
    faacConfig->bandWidth     = lowpass;
    faacConfig->quantqual     = (unsigned long) (getOutQuality() * 1000.0);
    faacConfig->outputFormat  = 1;
    faacConfig->inputFormat   = FAAC_INPUT_16BIT;

    if (!faacEncSetConfiguration(encoderHandle, faacConfig)) {
        throw Exception(__FILE__, __LINE__,
                        "error configuring faac library");
    }

    faacOpen = true;

    return true;
}


/*------------------------------------------------------------------------------
 *  Write data to the encoder
 *----------------------------------------------------------------------------*/
unsigned int
FaacEncoder :: write (  const void    * buf,
                        unsigned int    len )           throw ( Exception )
{
    if ( !isOpen() || len == 0 ) {
        return 0;
    }

    unsigned int    channels         = getInChannel();
    unsigned int    bitsPerSample    = getInBitsPerSample();
    unsigned int    sampleSize       = (bitsPerSample / 8) * channels;
    unsigned char * b                = (unsigned char*) buf;
    unsigned int    processed        = len - (len % sampleSize);
    unsigned int    nSamples         = processed / sampleSize;
    unsigned char * faacBuf          = new unsigned char[maxOutputBytes];
    int             samples          = (int) nSamples * channels;
    int             processedSamples = 0;

    while (processedSamples < samples) {
        int     outputBytes;
        int     inSamples = samples - processedSamples < (int) inputSamples
                          ? samples - processedSamples
                          : inputSamples;

        outputBytes = faacEncEncode(encoderHandle,
                                   (int32_t*) (b + processedSamples/sampleSize),
                                    inSamples,
                                    faacBuf,
                                    maxOutputBytes);
        getSink()->write(faacBuf, outputBytes);

        processedSamples += inSamples;
    }

    delete[] faacBuf;

    return processedSamples;
}


/*------------------------------------------------------------------------------
 *  Flush the data from the encoder
 *----------------------------------------------------------------------------*/
void
FaacEncoder :: flush ( void )
                                                            throw ( Exception )
{
    if ( !isOpen() ) {
        return;
    }

    getSink()->flush();
}


/*------------------------------------------------------------------------------
 *  Close the encoding session
 *----------------------------------------------------------------------------*/
void
FaacEncoder :: close ( void )                           throw ( Exception )
{
    if ( isOpen() ) {
        flush();
        faacEncClose(encoderHandle);
        faacOpen = false;

        getSink()->close();
    }
}


#endif // HAVE_FAAC_LIB

