/*------------------------------------------------------------------------------

   Copyright (c) 2000 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : DarkIce.cpp
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

/* ============================================================ include files */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#else
#error need stdlib.h
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#else
#error need unistd.h
#endif

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#else
#error need sys/types.h
#endif

#ifdef HAVE_SYS_WAIT_H
#include <sys/wait.h>
#else
#error need sys/wait.h
#endif

#ifdef HAVE_ERRNO_H
#include <errno.h>
#else
#error need errno.h
#endif

#ifdef HAVE_SCHED_H
#include <sched.h>
#else
#error need sched.h
#endif



#include "Util.h"
#include "IceCast.h"
#include "IceCast2.h"
#include "ShoutCast.h"
#include "FileCast.h"
#include "MultiThreadedConnector.h"
#include "DarkIce.h"

#ifdef HAVE_LAME_LIB
#include "LameLibEncoder.h"
#endif

#ifdef HAVE_VORBIS_LIB
#include "VorbisLibEncoder.h"
#endif


/* ===================================================  local data structures */


/* ================================================  local constants & macros */

/*------------------------------------------------------------------------------
 *  File identity
 *----------------------------------------------------------------------------*/
static const char fileid[] = "$Id$";


/*------------------------------------------------------------------------------
 *  Make sure wait-related stuff is what we expect
 *----------------------------------------------------------------------------*/
#ifndef WEXITSTATUS
# define WEXITSTATUS(stat_val)      ((unsigned)(stat_val) >> 8)
#endif
#ifndef WIFEXITED
# define WIFEXITED(stat_val)        (((stat_val) & 255) == 0)
#endif



/* ===============================================  local function prototypes */


/* =============================================================  module code */

/*------------------------------------------------------------------------------
 *  Initialize the object
 *----------------------------------------------------------------------------*/
void
DarkIce :: init ( const Config      & config )              throw ( Exception )
{
    unsigned int             bufferSecs;
    const ConfigSection    * cs;
    const char             * str;
    unsigned int             sampleRate;
    unsigned int             bitsPerSample;
    unsigned int             channel;
    bool                     reconnect;
    const char             * device;

    // the [general] section
    if ( !(cs = config.get( "general")) ) {
        throw Exception( __FILE__, __LINE__, "no section [general] in config");
    }
    str = cs->getForSure( "duration", " missing in section [general]");
    duration = Util::strToL( str);
    str = cs->getForSure( "bufferSecs", " missing in section [general]");
    bufferSecs = Util::strToL( str);
    str           = cs->get( "reconnect");
    reconnect     = str ? (Util::strEq( str, "yes") ? true : false) : true;


    // the [input] section
    if ( !(cs = config.get( "input")) ) {
        throw Exception( __FILE__, __LINE__, "no section [general] in config");
    }
    
    str        = cs->getForSure( "sampleRate", " missing in section [input]");
    sampleRate = Util::strToL( str);
    str       = cs->getForSure( "bitsPerSample", " missing in section [input]");
    bitsPerSample = Util::strToL( str);
    str           = cs->getForSure( "channel", " missing in section [input]");
    channel       = Util::strToL( str);
    device        = cs->getForSure( "device", " missing in section [input]");

    dsp             = AudioSource::createDspSource( device,
                                                    sampleRate,
                                                    bitsPerSample,
                                                    channel );
    encConnector    = new MultiThreadedConnector( dsp.get(), reconnect );

    noAudioOuts = 0;
    configIceCast( config, bufferSecs);
    configIceCast2( config, bufferSecs);
    configShoutCast( config, bufferSecs);
    configFileCast( config);
}


/*------------------------------------------------------------------------------
 *  Look for the IceCast stream outputs in the config file
 *----------------------------------------------------------------------------*/
void
DarkIce :: configIceCast (  const Config      & config,
                            unsigned int        bufferSecs  )
                                                        throw ( Exception )
{
    // look for IceCast encoder output streams,
    // sections [icecast-0], [icecast-1], ...
    char            stream[]        = "icecast- ";
    size_t          streamLen       = Util::strLen( stream);
    unsigned int    u;

    for ( u = noAudioOuts; u < maxOutput; ++u ) {
        const ConfigSection    * cs;

        // ugly hack to change the section name to "stream0", "stream1", etc.
        stream[streamLen-1] = '0' + (u - noAudioOuts);

        if ( !(cs = config.get( stream)) ) {
            break;
        }

#ifndef HAVE_LAME_LIB
        throw Exception( __FILE__, __LINE__,
                         "DarkIce not compiled with lame support, "
                         "thus can't connect to IceCast 1.x, stream: ",
                         stream);
#else

        const char                * str;

        unsigned int                sampleRate      = 0;
        unsigned int                channel         = 0;
        AudioEncoder::BitrateMode   bitrateMode;
        unsigned int                bitrate         = 0;
        double                      quality         = 0.0;
        const char                * server          = 0;
        unsigned int                port            = 0;
        const char                * password        = 0;
        const char                * mountPoint      = 0;
        const char                * remoteDumpFile  = 0;
        const char                * name            = 0;
        const char                * description     = 0;
        const char                * url             = 0;
        const char                * genre           = 0;
        bool                        isPublic        = false;
        int                         lowpass         = 0;
        int                         highpass        = 0;
        const char                * localDumpName   = 0;
        FileSink                  * localDumpFile   = 0;
        bool                        fileAddDate     = false;

        str         = cs->get( "sampleRate");
        sampleRate  = str ? Util::strToL( str) : dsp->getSampleRate();
        str         = cs->get( "channel");
        channel     = str ? Util::strToL( str) : dsp->getChannel();

        str         = cs->get( "bitrate");
        bitrate     = str ? Util::strToL( str) : 0;
        str         = cs->get( "quality");
        quality     = str ? Util::strToD( str) : 0.0;
        
        str         = cs->getForSure( "bitrateMode",
                                      " not specified in section ",
                                      stream);
        if ( Util::strEq( str, "cbr") ) {
            bitrateMode = AudioEncoder::cbr;
            
            if ( bitrate == 0 ) {
                throw Exception( __FILE__, __LINE__,
                                 "bitrate not specified for CBR encoding");
            }
            if ( cs->get( "quality" ) == 0 ) {
                throw Exception( __FILE__, __LINE__,
                                 "quality not specified for CBR encoding");
            }
        } else if ( Util::strEq( str, "abr") ) {
            bitrateMode = AudioEncoder::abr;

            if ( bitrate == 0 ) {
                throw Exception( __FILE__, __LINE__,
                                 "bitrate not specified for ABR encoding");
            }
        } else if ( Util::strEq( str, "vbr") ) {
            bitrateMode = AudioEncoder::vbr;

            if ( cs->get( "quality" ) == 0 ) {
                throw Exception( __FILE__, __LINE__,
                                 "quality not specified for VBR encoding");
            }
        } else {
            throw Exception( __FILE__, __LINE__,
                             "invalid bitrate mode: ", str);
        }

        server      = cs->getForSure( "server", " missing in section ", stream);
        str         = cs->getForSure( "port", " missing in section ", stream);
        port        = Util::strToL( str);
        password    = cs->getForSure("password"," missing in section ",stream);
        mountPoint  = cs->getForSure( "mountPoint",
                                      " missing in section ",
                                      stream);
        remoteDumpFile = cs->get( "remoteDumpFile");
        name        = cs->get( "name");
        description = cs->get("description");
        url         = cs->get( "url");
        genre       = cs->get( "genre");
        str         = cs->get( "public");
        isPublic    = str ? (Util::strEq( str, "yes") ? true : false) : false;
        str         = cs->get( "lowpass");
        lowpass     = str ? Util::strToL( str) : 0;
        str         = cs->get( "highpass");
        highpass    = str ? Util::strToL( str) : 0;
        str         = cs->get("fileAddDate");
        fileAddDate = str ? (Util::strEq( str, "yes") ? true : false) : false;

        localDumpName = cs->get( "localDumpFile");

        // go on and create the things

        // check for and create the local dump file if needed
        if ( localDumpName != 0 ) {
            if ( fileAddDate ) {
                localDumpName = Util::fileAddDate(localDumpName);
            }

            localDumpFile = new FileSink( localDumpName);
            if ( !localDumpFile->exists() ) {
                if ( !localDumpFile->create() ) {
                    reportEvent( 1, "can't create local dump file",
                                    localDumpName);
                    localDumpFile = 0;
                }
            }
            if ( fileAddDate ) {
                delete[] localDumpFile;
            }
        }
        // streaming related stuff
        audioOuts[u].socket = new TcpSocket( server, port);
        audioOuts[u].server = new IceCast( audioOuts[u].socket.get(),
                                           password,
                                           mountPoint,
                                           bitrate,
                                           name,
                                           description,
                                           url,
                                           genre,
                                           isPublic,
                                           remoteDumpFile,
                                           localDumpFile,
                                           bufferSecs );

        audioOuts[u].encoder = new LameLibEncoder( audioOuts[u].server.get(),
                                                   dsp.get(),
                                                   bitrateMode,
                                                   bitrate,
                                                   quality,
                                                   sampleRate,
                                                   channel,
                                                   lowpass,
                                                   highpass );

        encConnector->attach( audioOuts[u].encoder.get());
#endif // HAVE_LAME_LIB
    }

    noAudioOuts += u;
}


/*------------------------------------------------------------------------------
 *  Look for the IceCast2 stream outputs in the config file
 *----------------------------------------------------------------------------*/
void
DarkIce :: configIceCast2 (  const Config      & config,
                             unsigned int        bufferSecs  )
                                                        throw ( Exception )
{
    // look for IceCast2 encoder output streams,
    // sections [icecast2-0], [icecast2-1], ...
    char            stream[]        = "icecast2- ";
    size_t          streamLen       = Util::strLen( stream);
    unsigned int    u;

    for ( u = noAudioOuts; u < maxOutput; ++u ) {
        const ConfigSection    * cs;

        // ugly hack to change the section name to "stream0", "stream1", etc.
        stream[streamLen-1] = '0' + (u - noAudioOuts);

        if ( !(cs = config.get( stream)) ) {
            break;
        }

        const char                * str;

        IceCast2::StreamFormat      format;
        unsigned int                sampleRate      = 0;
        unsigned int                channel         = 0;
        AudioEncoder::BitrateMode   bitrateMode;
        unsigned int                bitrate         = 0;
        unsigned int                maxBitrate      = 0;
        double                      quality         = 0.0;
        const char                * server          = 0;
        unsigned int                port            = 0;
        const char                * password        = 0;
        const char                * mountPoint      = 0;
        const char                * name            = 0;
        const char                * description     = 0;
        const char                * url             = 0;
        const char                * genre           = 0;
        bool                        isPublic        = false;
        const char                * localDumpName   = 0;
        FileSink                  * localDumpFile   = 0;
        bool                        fileAddDate     = false;

        str         = cs->getForSure( "format", " missing in section ", stream);
        if ( Util::strEq( str, "vorbis") ) {
            format = IceCast2::oggVorbis;
        } else if ( Util::strEq( str, "mp3") ) {
            format = IceCast2::mp3;
        } else {
            throw Exception( __FILE__, __LINE__,
                             "unsupported stream format: ", str);
        }
                
        str         = cs->get( "sampleRate");
        sampleRate  = str ? Util::strToL( str) : dsp->getSampleRate();
        str         = cs->get( "channel");
        channel     = str ? Util::strToL( str) : dsp->getChannel();

        // determine fixed bitrate or variable bitrate quality
        str         = cs->get( "bitrate");
        bitrate     = str ? Util::strToL( str) : 0;
        str         = cs->get( "maxBitrate");
        maxBitrate  = str ? Util::strToL( str) : 0;
        str         = cs->get( "quality");
        quality     = str ? Util::strToD( str) : 0.0;
        
        str         = cs->getForSure( "bitrateMode",
                                      " not specified in section ",
                                      stream);
        if ( Util::strEq( str, "cbr") ) {
            bitrateMode = AudioEncoder::cbr;
            
            if ( bitrate == 0 ) {
                throw Exception( __FILE__, __LINE__,
                                 "bitrate not specified for CBR encoding");
            }
        } else if ( Util::strEq( str, "abr") ) {
            bitrateMode = AudioEncoder::abr;

            if ( bitrate == 0 ) {
                throw Exception( __FILE__, __LINE__,
                                 "bitrate not specified for ABR encoding");
            }
        } else if ( Util::strEq( str, "vbr") ) {
            bitrateMode = AudioEncoder::vbr;

            if ( cs->get( "quality" ) == 0 ) {
                throw Exception( __FILE__, __LINE__,
                                 "quality not specified for VBR encoding");
            }
        } else {
            throw Exception( __FILE__, __LINE__,
                             "invalid bitrate mode: ", str);
        }

        server      = cs->getForSure( "server", " missing in section ", stream);
        str         = cs->getForSure( "port", " missing in section ", stream);
        port        = Util::strToL( str);
        password    = cs->getForSure("password"," missing in section ",stream);
        mountPoint  = cs->getForSure( "mountPoint",
                                      " missing in section ",
                                      stream);
        name        = cs->get( "name");
        description = cs->get( "description");
        url         = cs->get( "url");
        genre       = cs->get( "genre");
        str         = cs->get( "public");
        isPublic    = str ? (Util::strEq( str, "yes") ? true : false) : false;
        str         = cs->get( "fileAddDate");
        fileAddDate = str ? (Util::strEq( str, "yes") ? true : false) : false;
        
        localDumpName = cs->get( "localDumpFile");

        // go on and create the things

        // check for and create the local dump file if needed
        if ( localDumpName != 0 ) {
            if ( fileAddDate ) {
                localDumpName = Util::fileAddDate(localDumpName);
            }

            localDumpFile = new FileSink( localDumpName);
            if ( !localDumpFile->exists() ) {
                if ( !localDumpFile->create() ) {
                    reportEvent( 1, "can't create local dump file",
                                    localDumpName);
                    localDumpFile = 0;
                }
            }
            if ( fileAddDate ) {
                delete[] localDumpName;
            }
        }

        // streaming related stuff
        audioOuts[u].socket = new TcpSocket( server, port);
        audioOuts[u].server = new IceCast2( audioOuts[u].socket.get(),
                                            password,
                                            mountPoint,
                                            format,
                                            bitrate,
                                            name,
                                            description,
                                            url,
                                            genre,
                                            isPublic,
                                            localDumpFile,
                                            bufferSecs );

        switch ( format ) {
            case IceCast2::mp3:
#ifndef HAVE_LAME_LIB
                throw Exception( __FILE__, __LINE__,
                                 "DarkIce not compiled with lame support, "
                                 "thus can't create mp3 stream: ",
                                 stream);
#else
                audioOuts[u].encoder = new LameLibEncoder(
                                                    audioOuts[u].server.get(),
                                                    dsp.get(),
                                                    bitrateMode,
                                                    bitrate,
                                                    quality,
                                                    sampleRate,
                                                    channel );
#endif // HAVE_LAME_LIB
                break;

            case IceCast2::oggVorbis:
#ifndef HAVE_VORBIS_LIB
                throw Exception( __FILE__, __LINE__,
                                "DarkIce not compiled with Ogg Vorbis support, "
                                "thus can't Ogg Vorbis stream: ",
                                stream);
#else
                audioOuts[u].encoder = new VorbisLibEncoder(
                                                audioOuts[u].server.get(),
                                                dsp.get(),
                                                bitrateMode,
                                                bitrate,
                                                quality,
                                                sampleRate,
                                                dsp->getChannel(),
                                                maxBitrate);
#endif // HAVE_VORBIS_LIB
                break;

            default:
                throw Exception( __FILE__, __LINE__,
                                "Illegal stream format: ", format);
        }

        encConnector->attach( audioOuts[u].encoder.get());
    }

    noAudioOuts += u;
}


/*------------------------------------------------------------------------------
 *  Look for the ShoutCast stream outputs in the config file
 *----------------------------------------------------------------------------*/
void
DarkIce :: configShoutCast (    const Config      & config,
                                unsigned int        bufferSecs  )
                                                        throw ( Exception )
{
    // look for Shoutcast encoder output streams,
    // sections [shoutcast-0], [shoutcast-1], ...
    char            stream[]        = "shoutcast- ";
    size_t          streamLen       = Util::strLen( stream);
    unsigned int    u;

    for ( u = noAudioOuts; u < maxOutput; ++u ) {
        const ConfigSection    * cs;

        // ugly hack to change the section name to "stream0", "stream1", etc.
        stream[streamLen-1] = '0' + (u - noAudioOuts);

        if ( !(cs = config.get( stream)) ) {
            break;
        }

#ifndef HAVE_LAME_LIB
        throw Exception( __FILE__, __LINE__,
                         "DarkIce not compiled with lame support, "
                         "thus can't connect to ShoutCast, stream: ",
                         stream);
#else

        const char                * str;

        unsigned int                sampleRate      = 0;
        unsigned int                channel         = 0;
        AudioEncoder::BitrateMode   bitrateMode;
        unsigned int                bitrate         = 0;
        double                      quality         = 0.0;
        const char                * server          = 0;
        unsigned int                port            = 0;
        const char                * password        = 0;
        const char                * name            = 0;
        const char                * url             = 0;
        const char                * genre           = 0;
        bool                        isPublic        = false;
        int                         lowpass         = 0;
        int                         highpass        = 0;
        const char                * irc             = 0;
        const char                * aim             = 0;
        const char                * icq             = 0;
        const char                * localDumpName   = 0;
        FileSink                  * localDumpFile   = 0;
        bool                        fileAddDate     = false;

        str         = cs->get( "sampleRate");
        sampleRate  = str ? Util::strToL( str) : dsp->getSampleRate();
        str         = cs->get( "channel");
        channel     = str ? Util::strToL( str) : dsp->getChannel();

        str         = cs->get( "bitrate");
        bitrate     = str ? Util::strToL( str) : 0;
        str         = cs->get( "quality");
        quality     = str ? Util::strToD( str) : 0.0;
        
        str         = cs->getForSure( "bitrateMode",
                                      " not specified in section ",
                                      stream);
        if ( Util::strEq( str, "cbr") ) {
            bitrateMode = AudioEncoder::cbr;
            
            if ( bitrate == 0 ) {
                throw Exception( __FILE__, __LINE__,
                                 "bitrate not specified for CBR encoding");
            }
            if ( cs->get( "quality" ) == 0 ) {
                throw Exception( __FILE__, __LINE__,
                                 "quality not specified for CBR encoding");
            }
        } else if ( Util::strEq( str, "abr") ) {
            bitrateMode = AudioEncoder::abr;

            if ( bitrate == 0 ) {
                throw Exception( __FILE__, __LINE__,
                                 "bitrate not specified for ABR encoding");
            }
        } else if ( Util::strEq( str, "vbr") ) {
            bitrateMode = AudioEncoder::vbr;

            if ( cs->get( "quality" ) == 0 ) {
                throw Exception( __FILE__, __LINE__,
                                 "quality not specified for VBR encoding");
            }
        } else {
            throw Exception( __FILE__, __LINE__,
                             "invalid bitrate mode: ", str);
        }

        server      = cs->getForSure( "server", " missing in section ", stream);
        str         = cs->getForSure( "port", " missing in section ", stream);
        port        = Util::strToL( str);
        password    = cs->getForSure("password"," missing in section ",stream);
        name        = cs->get( "name");
        url         = cs->get( "url");
        genre       = cs->get( "genre");
        str         = cs->get( "public");
        isPublic    = str ? (Util::strEq( str, "yes") ? true : false) : false;
        str         = cs->get( "lowpass");
        lowpass     = str ? Util::strToL( str) : 0;
        str         = cs->get( "highpass");
        highpass    = str ? Util::strToL( str) : 0;
        irc         = cs->get( "irc");
        aim         = cs->get( "aim");
        icq         = cs->get( "icq");
        str         = cs->get("fileAddDate");
        fileAddDate = str ? (Util::strEq( str, "yes") ? true : false) : false;

        localDumpName = cs->get( "localDumpFile");

        // go on and create the things

        // check for and create the local dump file if needed
        if ( localDumpName != 0 ) {
            if ( fileAddDate ) {
                localDumpName = Util::fileAddDate(localDumpName);
            }

            localDumpFile = new FileSink( localDumpName);
            if ( !localDumpFile->exists() ) {
                if ( !localDumpFile->create() ) {
                    reportEvent( 1, "can't create local dump file",
                                    localDumpName);
                    localDumpFile = 0;
                }
            }
            if ( fileAddDate ) {
                delete[] localDumpFile;
            }
        }

        // streaming related stuff
        audioOuts[u].socket = new TcpSocket( server, port);
        audioOuts[u].server = new ShoutCast( audioOuts[u].socket.get(),
                                             password,
                                             bitrate,
                                             name,
                                             url,
                                             genre,
                                             isPublic,
                                             irc,
                                             aim,
                                             icq,
                                             localDumpFile,
                                             bufferSecs );

        audioOuts[u].encoder = new LameLibEncoder( audioOuts[u].server.get(),
                                                   dsp.get(),
                                                   bitrateMode,
                                                   bitrate,
                                                   quality,
                                                   sampleRate,
                                                   channel,
                                                   lowpass,
                                                   highpass );

        encConnector->attach( audioOuts[u].encoder.get());
#endif // HAVE_LAME_LIB
    }

    noAudioOuts += u;
}


/*------------------------------------------------------------------------------
 *  Look for the FileCast stream outputs in the config file
 *----------------------------------------------------------------------------*/
void
DarkIce :: configFileCast (  const Config      & config )
                                                        throw ( Exception )
{
    // look for FileCast encoder output streams,
    // sections [file-0], [file-1], ...
    char            stream[]        = "file- ";
    size_t          streamLen       = Util::strLen( stream);
    unsigned int    u;

    for ( u = noAudioOuts; u < maxOutput; ++u ) {
        const ConfigSection    * cs;

        // ugly hack to change the section name to "stream0", "stream1", etc.
        stream[streamLen-1] = '0' + (u - noAudioOuts);

        if ( !(cs = config.get( stream)) ) {
            break;
        }

        const char                * str;

        const char                * format          = 0;
        AudioEncoder::BitrateMode   bitrateMode;
        unsigned int                bitrate         = 0;
        double                      quality         = 0.0;
        const char                * targetFileName  = 0;
        unsigned int                sampleRate      = 0;
        int                         lowpass         = 0;
        int                         highpass        = 0;

        format      = cs->getForSure( "format", " missing in section ", stream);
        if ( !Util::strEq( format, "vorbis") && !Util::strEq( format, "mp3") ) {
            throw Exception( __FILE__, __LINE__,
                             "unsupported stream format: ", format);
        }

        str         = cs->getForSure("bitrate", " missing in section ", stream);
        bitrate     = Util::strToL( str);
        targetFileName    = cs->getForSure( "fileName",
                                            " missing in section ",
                                            stream);
        str         = cs->get( "sampleRate");
        sampleRate  = str ? Util::strToL( str) : dsp->getSampleRate();

        str         = cs->get( "bitrate");
        bitrate     = str ? Util::strToL( str) : 0;
        str         = cs->get( "quality");
        quality     = str ? Util::strToD( str) : 0.0;
        
        str         = cs->getForSure( "bitrateMode",
                                      " not specified in section ",
                                      stream);
        if ( Util::strEq( str, "cbr") ) {
            bitrateMode = AudioEncoder::cbr;
            
            if ( bitrate == 0 ) {
                throw Exception( __FILE__, __LINE__,
                                 "bitrate not specified for CBR encoding");
            }
            if ( cs->get( "quality" ) == 0 ) {
                throw Exception( __FILE__, __LINE__,
                                 "quality not specified for CBR encoding");
            }
        } else if ( Util::strEq( str, "abr") ) {
            bitrateMode = AudioEncoder::abr;

            if ( bitrate == 0 ) {
                throw Exception( __FILE__, __LINE__,
                                 "bitrate not specified for ABR encoding");
            }
        } else if ( Util::strEq( str, "vbr") ) {
            bitrateMode = AudioEncoder::vbr;

            if ( cs->get( "quality" ) == 0 ) {
                throw Exception( __FILE__, __LINE__,
                                 "quality not specified for VBR encoding");
            }
        } else {
            throw Exception( __FILE__, __LINE__,
                             "invalid bitrate mode: ", str);
        }

        str         = cs->get( "lowpass");
        lowpass     = str ? Util::strToL( str) : 0;
        str         = cs->get( "highpass");
        highpass    = str ? Util::strToL( str) : 0;

        // go on and create the things

        // the underlying file
        FileSink  * targetFile = new FileSink( targetFileName);
        if ( !targetFile->exists() ) {
            if ( !targetFile->create() ) {
                throw Exception( __FILE__, __LINE__,
                                 "can't create output file", targetFileName);
            }
        }

        // streaming related stuff
        audioOuts[u].socket = 0;
        audioOuts[u].server = new FileCast( targetFile );

        if ( Util::strEq( format, "mp3") ) {
#ifndef HAVE_LAME_LIB
                throw Exception( __FILE__, __LINE__,
                                 "DarkIce not compiled with lame support, "
                                 "thus can't create mp3 stream: ",
                                 stream);
#else
                audioOuts[u].encoder = new LameLibEncoder(
                                                    audioOuts[u].server.get(),
                                                    dsp.get(),
                                                    bitrateMode,
                                                    bitrate,
                                                    quality,
                                                    sampleRate,
                                                    dsp->getChannel(),
                                                    lowpass,
                                                    highpass );
#endif // HAVE_LAME_LIB
        } else if ( Util::strEq( format, "vorbis") ) {
#ifndef HAVE_VORBIS_LIB
                throw Exception( __FILE__, __LINE__,
                                "DarkIce not compiled with Ogg Vorbis support, "
                                "thus can't Ogg Vorbis stream: ",
                                stream);
#else
                audioOuts[u].encoder = new VorbisLibEncoder(
                                                    audioOuts[u].server.get(),
                                                    dsp.get(),
                                                    bitrateMode,
                                                    bitrate,
                                                    quality,
                                                    dsp->getSampleRate(),
                                                    dsp->getChannel() );
#endif // HAVE_VORBIS_LIB
        } else {
                throw Exception( __FILE__, __LINE__,
                                "Illegal stream format: ", format);
        }

        encConnector->attach( audioOuts[u].encoder.get());
    }

    noAudioOuts += u;
}


/*------------------------------------------------------------------------------
 *  Set POSIX real-time scheduling, if super-user
 *----------------------------------------------------------------------------*/
void
DarkIce :: setRealTimeScheduling ( void )               throw ( Exception )
{
// Only if the OS has the POSIX real-time scheduling functions implemented.
#if defined( HAVE_SCHED_GETSCHEDULER ) && defined( HAVE_SCHED_GETPARAM )
    uid_t   euid;

    euid = geteuid();

    if ( euid == 0 ) {
        int                 high_priority;
        struct sched_param  param;

        /* store the old scheduling parameters */
        if ( (origSchedPolicy = sched_getscheduler(0)) == -1 ) {
            throw Exception( __FILE__, __LINE__, "sched_getscheduler", errno);
        }

        if ( sched_getparam( 0, &param) == -1 ) {
            throw Exception( __FILE__, __LINE__, "sched_getparam", errno);
        }
        origSchedPriority = param.sched_priority;
        
        /* set SCHED_FIFO with max - 1 priority */
        if ( (high_priority = sched_get_priority_max(SCHED_FIFO)) == -1 ) {
            throw Exception(__FILE__,__LINE__,"sched_get_priority_max",errno);
        }
        reportEvent( 8, "scheduler high priority", high_priority);

        param.sched_priority = high_priority - 1;

        if ( sched_setscheduler( 0, SCHED_FIFO, &param) == -1 ) {
            throw Exception( __FILE__, __LINE__, "sched_setscheduler", errno);
        }

        /* ask the new priortiy and report it */
        if ( sched_getparam( 0, &param) == -1 ) {
            throw Exception( __FILE__, __LINE__, "sched_getparam", errno);
        }

        reportEvent( 1,
                     "Using POSIX real-time scheduling, priority",
                     param.sched_priority );
    } else {
        reportEvent( 1,
        "Not running as super-user, unable to use POSIX real-time scheduling" );
        reportEvent( 1,
        "It is recommended that you run this program as super-user");
    }
#else
    reportEvent( 1, "POSIX scheduling not supported on this system, "
                    "this may cause recording skips");
#endif // HAVE_SCHED_GETSCHEDULER && HAVE_SCHED_GETPARAM
}


/*------------------------------------------------------------------------------
 *  Set the original scheduling of the process, the one prior to the
 *  setRealTimeScheduling call.
 *  WARNING: make sure you don't call this before setRealTimeScheduling!!
 *----------------------------------------------------------------------------*/
void
DarkIce :: setOriginalScheduling ( void )               throw ( Exception )
{
// Only if the OS has the POSIX real-time scheduling functions implemented.
#if defined( HAVE_SCHED_GETSCHEDULER ) && defined( HAVE_SCHED_GETPARAM )
    uid_t   euid;

    euid = geteuid();

    if ( euid == 0 ) {
        struct sched_param  param;

        if ( sched_getparam( 0, &param) == -1 ) {
            throw Exception( __FILE__, __LINE__, "sched_getparam", errno);
        }

        param.sched_priority = origSchedPriority;

        if ( sched_setscheduler( 0, origSchedPolicy, &param) == -1 ) {
            throw Exception( __FILE__, __LINE__, "sched_setscheduler", errno);
        }

        reportEvent( 5, "reverted to original scheduling");
    }
#endif // HAVE_SCHED_GETSCHEDULER && HAVE_SCHED_GETPARAM
}


/*------------------------------------------------------------------------------
 *  Run the encoder
 *----------------------------------------------------------------------------*/
bool
DarkIce :: encode ( void )                          throw ( Exception )
{
    unsigned int       len;
    unsigned long      bytes;

    if ( !encConnector->open() ) {
        throw Exception( __FILE__, __LINE__, "can't open connector");
    }
    
    bytes = dsp->getSampleRate() *
            (dsp->getBitsPerSample() / 8UL) *
            dsp->getChannel() *
            duration;
                                                
    len = encConnector->transfer( bytes, 4096, 1, 0 );

    reportEvent( 1, len, "bytes transfered to the encoders");

    encConnector->close();

    return true;
}


/*------------------------------------------------------------------------------
 *  Run
 *----------------------------------------------------------------------------*/
int
DarkIce :: run ( void )                             throw ( Exception )
{
    reportEvent( 3, "encoding");
    setRealTimeScheduling();
    encode();
    setOriginalScheduling();
    reportEvent( 3, "encoding ends");

    return 0;
}


/*------------------------------------------------------------------------------
 
  $Source$

  $Log$
  Revision 1.43  2005/04/11 19:27:43  darkeye
  added option to turn off automatic reconnect feature

  Revision 1.42  2005/04/04 08:36:17  darkeye
  commited changes to enable Jack support
  thanks to Nicholas J. Humfrey, njh@ecs.soton.ac.uk

  Revision 1.41  2005/04/03 05:12:20  jbebel
  Changed mechanism for testing the presence of the quality value such that
  zero is a valid option.

  Revision 1.40  2004/02/23 19:12:51  darkeye
  ported to NetBSD

  Revision 1.39  2004/02/19 06:47:06  darkeye
  finalized OpenBSD port

  Revision 1.38  2004/02/18 21:08:11  darkeye
  ported to OpenBSD (real-time scheduling not yet supported)

  Revision 1.37  2004/02/15 12:14:38  darkeye
  added patch to allow mp3 stream downsampling to mono for icecast2 as well

  Revision 1.36  2004/02/15 12:06:30  darkeye
  added ALSA support, thanks to Christian Forster

  Revision 1.35  2003/02/09 13:15:57  darkeye
  added feature for setting the TITLE comment field for vorbis streams

  Revision 1.34  2003/02/09 12:57:36  darkeye
  cosmetic changes to the fileAddDate option

  Revision 1.33  2002/11/20 16:52:05  wandereq
  added fileAddDate function

  Revision 1.32  2002/10/19 12:24:55  darkeye
  anged internals so that now each encoding/server connection is
  a separate thread

  Revision 1.31  2002/08/20 19:35:37  darkeye
  added possibility to specify maximum bitrate for Ogg Vorbis streams

  Revision 1.30  2002/08/20 18:37:49  darkeye
  added mp3 streaming possibility for icecast2

  Revision 1.29  2002/08/03 12:41:18  darkeye
  added possibility to stream in mono when recording in stereo

  Revision 1.28  2002/07/20 10:59:00  darkeye
  added support for Ogg Vorbis 1.0, removed support for rc2

  Revision 1.27  2002/04/13 11:26:00  darkeye
  added cbr, abr and vbr setting feature with encoding quality

  Revision 1.26  2002/03/28 16:43:11  darkeye
  enabled resampling and variable bitrates for vorbis (icecast2) streams

  Revision 1.25  2002/02/28 09:49:25  darkeye
  added possibility to save the encoded stream to a local file only
  (no streaming server needed)

  Revision 1.24  2002/02/20 11:54:11  darkeye
  added local dump file possibility

  Revision 1.23  2002/02/20 10:35:35  darkeye
  updated to work with Ogg Vorbis libs rc3 and current IceCast2 cvs

  Revision 1.22  2001/10/20 10:56:45  darkeye
  added possibility to disable highpass and lowpass filters for lame

  Revision 1.21  2001/10/19 12:39:42  darkeye
  created configure options to compile with or without lame / Ogg Vorbis

  Revision 1.20  2001/10/19 09:03:39  darkeye
  added support for resampling mp3 streams

  Revision 1.19  2001/09/14 19:31:06  darkeye
  added IceCast2 / vorbis support

  Revision 1.18  2001/09/11 15:05:21  darkeye
  added Solaris support

  Revision 1.17  2001/09/09 11:27:31  darkeye
  added support for ShoutCast servers

  Revision 1.16  2001/09/05 20:11:15  darkeye
  removed dependency on locally stored SGI STL header files
  now compiler-supplied C++ library STL header files are used
  compiles under GNU C++ 3
  hash_map (an SGI extension to STL) replaced with map
  std:: namespace prefix added to all STL class references

  Revision 1.15  2001/08/30 17:25:56  darkeye
  renamed configure.h to config.h

  Revision 1.14  2001/08/29 21:08:30  darkeye
  made some description options in the darkice config file optional

  Revision 1.13  2001/08/26 20:44:30  darkeye
  removed external command-line encoder support
  replaced it with a shared-object support for lame with the possibility
  of static linkage

  Revision 1.12  2000/12/20 12:36:47  darkeye
  added POSIX real-time scheduling

  Revision 1.11  2000/11/18 11:13:27  darkeye
  removed direct reference to cout, except from main.cpp
  all class use the Reporter interface to report events

  Revision 1.10  2000/11/17 15:50:48  darkeye
  added -Wall flag to compiler and eleminated new warnings

  Revision 1.9  2000/11/15 18:37:37  darkeye
  changed the transferable number of bytes to unsigned long

  Revision 1.8  2000/11/15 18:08:43  darkeye
  added multiple verbosity-level event reporting and verbosity command
  line option

  Revision 1.7  2000/11/13 19:38:55  darkeye
  moved command line parameter parsing from DarkIce.cpp to main.cpp

  Revision 1.6  2000/11/13 18:46:50  darkeye
  added kdoc-style documentation comments

  Revision 1.5  2000/11/10 20:16:21  darkeye
  first real tests with multiple streaming

  Revision 1.4  2000/11/09 22:09:46  darkeye
  added multiple outputs
  added configuration reading
  added command line processing

  Revision 1.3  2000/11/08 17:29:50  darkeye
  added configuration file reader

  Revision 1.2  2000/11/05 14:08:27  darkeye
  changed builting to an automake / autoconf environment

  Revision 1.1.1.1  2000/11/05 10:05:49  darkeye
  initial version

  
------------------------------------------------------------------------------*/

