/*------------------------------------------------------------------------------

   Copyright (c) 2005 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : aacPlusEncoder.cpp
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

// compile the whole file only if aacplus support configured in
#ifdef HAVE_AACPLUS_LIB



#include "Exception.h"
#include "Util.h"
#include "aacPlusEncoder.h"


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
aacPlusEncoder :: open ( void )
                                                            throw ( Exception )
{
    if ( isOpen() ) {
        close();
    }

    // open the underlying sink
    if ( !sink->open() ) {
        throw Exception( __FILE__, __LINE__,
                         "aacplus lib opening underlying sink error");
    }

    reportEvent(1, "Using aacplus codec version", "720 3gpp");
    
    bitrate = getOutBitrate() * 1000;
    bandwidth = 0;
    useParametricStereo = 0;
    numAncDataBytes=0;
    coreWriteOffset = 0;
    envReadOffset = 0;
    writeOffset = INPUT_DELAY*MAX_CHANNELS;
    writtenSamples = 0;
    aacEnc = NULL;
    hEnvEnc=NULL;

    /* set up basic parameters for aacPlus codec */
    AacInitDefaultConfig(&config);
    nChannelsAAC = nChannelsSBR = getOutChannel();
    
    if ( (getInChannel() == 2) && (bitrate >= 16000) && (bitrate < 44001) ) {
        useParametricStereo = 1;
        nChannelsAAC = 1;
        nChannelsSBR = 2;
        
        reportEvent(10, "use Parametric Stereo");
        
        envReadOffset = (MAX_DS_FILTER_DELAY + INPUT_DELAY)*MAX_CHANNELS;
        coreWriteOffset = CORE_INPUT_OFFSET_PS;
        writeOffset = envReadOffset;
    } else {
    	/* set up 2:1 downsampling */
    	InitIIR21_Resampler(&(IIR21_reSampler[0]));
    	InitIIR21_Resampler(&(IIR21_reSampler[1]));
    	
    	if(IIR21_reSampler[0].delay > MAX_DS_FILTER_DELAY)
    		throw Exception(__FILE__, __LINE__, "IIR21 resampler delay is bigger then MAX_DS_FILTER_DELAY");
        writeOffset += IIR21_reSampler[0].delay*MAX_CHANNELS;
    }
    
    sampleRateAAC = getInSampleRate();
    config.bitRate = bitrate;
    config.nChannelsIn=getInChannel();
    config.nChannelsOut=nChannelsAAC;
    config.bandWidth=bandwidth;
    
    /* set up SBR configuration    */
    if(!IsSbrSettingAvail(bitrate, nChannelsAAC, sampleRateAAC, &sampleRateAAC))
        throw Exception(__FILE__, __LINE__, "No valid SBR configuration found");
    
    InitializeSbrDefaults (&sbrConfig);
    sbrConfig.usePs = useParametricStereo;
    
    AdjustSbrSettings( &sbrConfig,
                       bitrate,
                       nChannelsAAC,
                       sampleRateAAC,
                       AACENC_TRANS_FAC,
                       24000);
    
    EnvOpen( &hEnvEnc,
             inBuf + coreWriteOffset,
             &sbrConfig,
             &config.bandWidth);
        
    /* set up AAC encoder, now that samling rate is known */
    config.sampleRate = sampleRateAAC;
    if (AacEncOpen(&aacEnc, config) != 0){
        AacEncClose(aacEnc);
        throw Exception(__FILE__, __LINE__, "Initialisation of AAC failed !");
    }
    
    init_plans();
    
    /* create the ADTS header */
    adts_hdr(outBuf, &config);
    
    inSamples = AACENC_BLOCKSIZE * getInChannel() * 2;
    
    aacplusOpen = true;
    reportEvent(10, "bitrate=", bitrate);
    reportEvent(10, "nChannelsIn", getInChannel());
    reportEvent(10, "nChannelsOut", getOutChannel());
    reportEvent(10, "nChannelsSBR", nChannelsSBR);
    reportEvent(10, "nChannelsAAC", nChannelsAAC);
    reportEvent(10, "sampleRateAAC", sampleRateAAC);
    reportEvent(10, "inSamples", inSamples);
    return true;
}


/*------------------------------------------------------------------------------
 *  Write data to the encoder
 *----------------------------------------------------------------------------*/
unsigned int
aacPlusEncoder :: write (  const void    * buf,
                        unsigned int    len )           throw ( Exception )
{
    if ( !isOpen() || len == 0) {
        return 0;
    }
    
    unsigned int    channels         = getInChannel();
    unsigned int    bitsPerSample    = getInBitsPerSample();
    unsigned int    sampleSize       = (bitsPerSample / 8) * channels;
    unsigned int    processed        = len - (len % sampleSize);
    unsigned int    nSamples         = processed / sampleSize;
    unsigned int    samples          = (unsigned int) nSamples * channels;
    
    
    
    
    unsigned int i; 
    int ch, outSamples, numOutBytes;


    reportEvent(10, "converting short to float");
    short *TimeDataPcm = (short *) buf;
    
    if(channels == 2) {
        for (i=0; i<samples; i++)
            inBuf[i+writeOffset+writtenSamples] = (float) TimeDataPcm[i];
    } else {
        /* using only left channel buffer for mono encoder */
        for (i=0; i<samples; i++)
            inBuf[writeOffset+2*writtenSamples+2*i] = (float) TimeDataPcm[i];
    }

    writtenSamples+=samples;
    reportEvent(10, "writtenSamples", writtenSamples);
    
    if (writtenSamples < inSamples)
        return samples;
    
    /* encode one SBR frame */
    reportEvent(10, "encode one SBR frame");
    EnvEncodeFrame( hEnvEnc,
                    inBuf + envReadOffset,
                    inBuf + coreWriteOffset,
                    MAX_CHANNELS,
                    &numAncDataBytes,
                    ancDataBytes);
    
    reportEvent(10, "numAncDataBytes=", numAncDataBytes);
    
    /* 2:1 downsampling for AAC core */
    if (!useParametricStereo) {
        reportEvent(10, "2:1 downsampling for AAC core");
        for( ch=0; ch<nChannelsAAC; ch++ )
            IIR21_Downsample( &(IIR21_reSampler[ch]),
                              inBuf + writeOffset+ch,
                              writtenSamples/channels,
                              MAX_CHANNELS,
                              inBuf+ch,
                              &outSamples,
                              MAX_CHANNELS);
        
        reportEvent(10, "outSamples=", outSamples);
    }
        
    /* encode one AAC frame */
    reportEvent(10, "encode one AAC frame");
    AacEncEncode( aacEnc,
                  inBuf,
                  useParametricStereo ? 1 : MAX_CHANNELS, /* stride (step) */
                  ancDataBytes,
                  &numAncDataBytes,
                  (unsigned *) (outBuf+ADTS_HEADER_SIZE),
                  &numOutBytes);
    if (useParametricStereo) {
        memcpy( inBuf,inBuf+AACENC_BLOCKSIZE,CORE_INPUT_OFFSET_PS*sizeof(float));
    } else {
        memmove( inBuf,inBuf+AACENC_BLOCKSIZE*2*MAX_CHANNELS,writeOffset*sizeof(float));
    }
    
    /* Write one frame of encoded audio */
    if (numOutBytes) {
    	reportEvent(10, "Write one frame of encoded audio:", numOutBytes+ADTS_HEADER_SIZE);
    	adts_hdr_up(outBuf, numOutBytes);
    	sink->write(outBuf, numOutBytes+ADTS_HEADER_SIZE);
    }
    
    writtenSamples=0;
    
    return samples;
}


/*------------------------------------------------------------------------------
 *  Flush the data from the encoder
 *----------------------------------------------------------------------------*/
void
aacPlusEncoder :: flush ( void )
                                                            throw ( Exception )
{
    if ( !isOpen() ) {
        return;
    }

    sink->flush();
}


/*------------------------------------------------------------------------------
 *  Close the encoding session
 *----------------------------------------------------------------------------*/
void
aacPlusEncoder :: close ( void )                           throw ( Exception )
{
    if ( isOpen() ) {
        flush();
    
        destroy_plans();
        AacEncClose(aacEnc);
        if (hEnvEnc) {
            EnvClose(hEnvEnc);
        }
    
        aacplusOpen = false;
    
        sink->close();
    }
}


#endif // HAVE_AACPLUS_LIB
