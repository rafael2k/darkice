bin_PROGRAMS = darkice
AM_CXXFLAGS = -O2 -pedantic -Wall @DEBUG_CXXFLAGS@ @PTHREAD_CFLAGS@
			  @JACK_CFLAGS@ 
INCLUDES = @LAME_INCFLAGS@ @VORBIS_INCFLAGS@ @OPUS_INCFLAGS@ @FAAC_INCFLAGS@ @AACPLUS_INCFLAGS@ @TWOLAME_INCFLAGS@ \
		@ALSA_INCFLAGS@ @PULSEAUDIO_INCFLAGS@ @JACK_INCFLAGS@ @SRC_INCFLAGS@
LDADD = @PTHREAD_LIBS@ @LAME_LDFLAGS@ @VORBIS_LDFLAGS@ @OPUS_LDFLAGS@ @FAAC_LDFLAGS@ @AACPLUS_LDFLAGS@ @TWOLAME_LDFLAGS@ \
		@ALSA_LDFLAGS@ @PULSEAUDIO_LDFLAGS@ @JACK_LDFLAGS@ @SRC_LDFLAGS@

if HAVE_SRC_LIB
AFLIB_SOURCE = 
else
AFLIB_SOURCE = aflibDebug.h\
                aflibDebug.cc\
                aflibConverter.h\
                aflibConverter.cc\
                aflibConverterLargeFilter.h\
                aflibConverterSmallFilter.h
endif

darkice_SOURCES =   AudioEncoder.h\
                    AudioSource.h\
                    AudioSource.cpp\
                    BufferedSink.cpp\
                    BufferedSink.h\
                    CastSink.cpp\
                    CastSink.h\
                    FileSink.h\
                    FileSink.cpp\
                    Connector.cpp\
                    Connector.h\
                    MultiThreadedConnector.cpp\
                    MultiThreadedConnector.h\
                    DarkIce.cpp\
                    DarkIce.h\
                    Exception.cpp\
                    Exception.h\
                    IceCast.cpp\
                    IceCast.h\
                    IceCast2.cpp\
                    IceCast2.h\
                    ShoutCast.cpp\
                    ShoutCast.h\
                    FileCast.h\
                    FileCast.cpp\
                    LameLibEncoder.cpp\
                    LameLibEncoder.h\
                    TwoLameLibEncoder.cpp\
                    TwoLameLibEncoder.h\
                    VorbisLibEncoder.cpp\
                    VorbisLibEncoder.h\
                    OpusLibEncoder.cpp\
                    OpusLibEncoder.h\
                    FaacEncoder.cpp\
                    FaacEncoder.h\
                    aacPlusEncoder.cpp\
                    aacPlusEncoder.h\
                    OssDspSource.cpp\
                    OssDspSource.h\
                    SerialUlaw.cpp\
                    SerialUlaw.h\
                    SolarisDspSource.cpp\
                    SolarisDspSource.h\
                    Ref.h\
                    Referable.h\
                    Sink.h\
                    Source.h\
                    TcpSocket.cpp\
                    TcpSocket.h\
                    Util.cpp\
                    Util.h\
                    ConfigSection.h\
                    ConfigSection.cpp\
                    DarkIceConfig.h\
                    DarkIceConfig.cpp\
                    Reporter.h\
                    Reporter.cpp\
                    AlsaDspSource.h\
                    AlsaDspSource.cpp\
                    PulseAudioDspSource.h\
                    PulseAudioDspSource.cpp\
                    JackDspSource.h\
                    JackDspSource.cpp\
                    main.cpp \
                    $(AFLIB_SOURCE)

EXTRA_darkice_SOURCES = aflibDebug.h\
                        aflibDebug.cc\
                        aflibConverter.h\
                        aflibConverter.cc\
                        aflibConverterLargeFilter.h\
                        aflibConverterSmallFilter.h
