#-------------------------------------------------------------------------------
#
#  Copyright (c) 2000 Tyrell Corporation. All rights reserved.
#
#  Tyrell DarkIce
#
#  File     : darkice.spec
#  Version  : $Revision$
#  Author   : $Author$
#  Location : $Source$
#  
#  Abstract : 
#
#   Specification file to build RPM packages of DarkIce
#
#  Copyright notice:
#
#   This program is free software; you can redistribute it and/or
#   modify it under the terms of the GNU General Public License  
#   as published by the Free Software Foundation; either version 2
#   of the License, or (at your option) any later version.
#  
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of 
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
#   GNU General Public License for more details.
#  
#   You should have received a copy of the GNU General Public License
#   along with this program; if not, write to the Free Software
#   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#
#-------------------------------------------------------------------------------

# ===================================================================== preamble
Summary : DarkIce live IceCast / ShoutCast streamer
Name: darkice
Vendor: Tyrell Hungary
Packager: Akos Maroy <darkeye@tyrell.hu>
Version: 0.15
Release: 1
Copyright: GPL
Group: Applications/Multimedia
Source: http://prdownloads.sourceforge.net/darkice/darkice-%{version}.tar.gz
URL: http://darkice.sourceforge.net/
Provides: darkice
BuildRoot: %{_tmppath}/%{name}-%{version}-root
Prefix: /usr

%description
DarkIce is an IceCast, IceCast2 and ShoutCast live audio streamer. It
takes audio input from a sound card, encodes it into mp3 and/or Ogg Vorbis,
and sends the mp3 stream to one or more IceCast and/or ShoutCast servers,
the Ogg Vorbis stream to one or more IceCast2 servers.

This RPM is compiled without ALSA support, as there are no official RPM
packages for ALSA. To utilize ALSA, you must compile darkice manually.



# =================================================================== prep stage
%prep
%setup


# ================================================================== build stage
%build
%configure --with-alsa=no
make all


# ================================================================ install stage
%install
%makeinstall


# ========================================================== pre-install scripts
%pre


# ========================================================= post-install scripts
%post


# ======================================================================== clean
%clean
rm -rf $RPM_BUILD_ROOT
make clean


# =========================================================== main package files
%files
%defattr (-, root, root)
%doc COPYING ChangeLog README TODO AUTHORS
%config %{_sysconfdir}/darkice.cfg
%{_bindir}/darkice
%{_mandir}/man1/darkice.1*
%{_mandir}/man5/darkice.cfg.5*


# =================================================================== change log
#
#   $Log$
#   Revision 1.1  2005/04/14 11:58:08  darkeye
#   moved directory rpm to etc/rpm
#
#   Revision 1.24  2005/04/14 11:11:59  darkeye
#   for version 0.15
#
#   Revision 1.23  2004/02/15 22:30:08  darkeye
#   for version 0.14
#
#   Revision 1.22  2004/02/15 12:06:29  darkeye
#   added ALSA support, thanks to Christian Forster
#
#   Revision 1.21  2004/01/07 22:14:44  darkeye
#   for release 0.13.2
#
#   Revision 1.20  2003/02/12 15:50:08  darkeye
#   for version 0.13.1
#
#   Revision 1.19  2003/02/09 15:09:41  darkeye
#   for version 0.13
#
#   Revision 1.18  2002/10/20 21:16:43  darkeye
#   for version 0.12
#
#   Revision 1.17  2002/08/20 20:45:08  darkeye
#   for version 0.11
#
#   Revision 1.16  2002/08/03 10:32:04  darkeye
#   for version 0.10.2beta
#
#   Revision 1.15  2002/08/02 18:17:13  darkeye
#   for version 0.10.1
#
#   Revision 1.14  2002/07/20 16:46:12  darkeye
#   for version 0.10
#
#   Revision 1.13  2002/04/09 13:10:43  darkeye
#   resolved memory leak issue introduced in 0.9
#
#   Revision 1.12  2002/03/28 17:11:18  darkeye
#   added file AUTHORS to set of distributed files
#
#   Revision 1.11  2002/03/28 16:56:14  darkeye
#   for version 0.9
#
#   Revision 1.10  2002/02/20 14:26:51  darkeye
#   version 0.8
#
#   Revision 1.9  2001/10/20 10:56:45  darkeye
#   added possibility to disable highpass and lowpass filters for lame
#
#   Revision 1.8  2001/10/19 12:48:03  darkeye
#   for new version
#
#   Revision 1.7  2001/09/18 18:00:02  darkeye
#   removed --enable-static configure option
#
#   Revision 1.6  2001/09/18 17:35:15  darkeye
#   for version 0.6
#
#   Revision 1.5  2001/09/13 05:06:41  darkeye
#   removed references to SourceForget FTP sites, as they are phased out
#
#   Revision 1.4  2001/09/09 12:26:33  darkeye
#   updated to reflect that DarkIce is now both an IceCast and ShoutCast streamer
#
#   Revision 1.3  2001/09/09 11:48:09  darkeye
#   added man page darkice.cfg.5
#
#   Revision 1.2  2001/09/02 14:44:14  darkeye
#   added system level configuration file
#
#   Revision 1.1  2001/09/02 12:46:05  darkeye
#   added RPM package creation scripts
#
#

