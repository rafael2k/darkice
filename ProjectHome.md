# 1. What Is DarkIce? #

DarkIce is a live audio streamer. It records audio from an audio interface (e.g. sound card), encodes it and sends it to a streaming server.

DarkIce can record from:

  * OSS audio devices
  * ALSA audio devices
  * Solaris audio interface
  * Jack sources
  * uLaw audio input through a serial interface
  * CoreAudio (branch darkice-macosx)
  * PulseAudio sources

DarkIce can encode in the following formats:

  * mp3 - using the lame library
  * mp2 - using the twolame library
  * Ogg/Vorbis
  * AAC - using the faac library
  * AAC HEv2 - using libaacplus library
  * Ogg/Opus

DarkIce can send the encoded stream to the following streaming servers:

  * ShoutCast
  * IceCast 1.3.x and 2.x
  * Darwin Streaming Server
  * archive the encoded audio in files


DarkIce runs on the following operating systems:

  * FreeBSD
  * Linux
  * MacOS X
  * NetBSD / OpenBSD
  * SUN Solaris
  * GNU/Hurd

DarkIce is being tested with:

  * GCC 4.4.2
  * Lame 3.98.2
  * libvorbis 1.2.3
  * libogg 1.1.4
  * faac 1.28
  * twolame 0.3.12
  * [libaacplus](http://tipok.org.ua/node/17) version 2.0.0 or greater
  * Jack 0.116.2
  * libopus 0.9.14

Tested in both little and big endian computers, 32 and 64bits.

DarkIce initial author is [Ákos Maróy](mailto:akosSTOP_SPAMmaroyDOThu).

DarkIce current author and maintainer is [Rafael Diniz](mailto:rafaelSTOP_SPAMriseupDOTnet).

# 2. Status #

DarkIce is considered stable, it has been used since fall 2000 in production 24 hours per day.

The latest version is 1.2, check out the [Downloads](http://code.google.com/p/darkice/downloads/list) section, and also the [ChangeLog](http://code.google.com/p/darkice/source/browse/darkice/trunk/ChangeLog).

There is a GTK frontend made by Rafael Diniz, called [DarkSnow](http://darksnow.radiolivre.org).

# 3. Discussion #

For questions and feedback on DarkIce, please join the darkice mailing list: http://www.freelists.org/list/darkice