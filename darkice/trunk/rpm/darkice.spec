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
Summary : DarkIce live IceCast streamer
Name: darkice
Vendor: Tyrell Hungary
Packager: Akos Maroy <darkeye@tyrell.hu>
Version: 0.4
Release: 1
Copyright: GPL
Group: Applications/Multimedia
Source: ftp://darkice.sourceforge.net/pub/darkice/darkice-%{version}.tar.gz
URL: http://darkice.sourceforge.net/
Provides: darkice
BuildRoot: %{_tmppath}/%{name}-%{version}-root
Prefix: /usr

%description
DarkIce  is an IceCast live audio streamer. It takes audio input from a sound
card, encodes it into  mp3, and sends the stream to an IceCast server.


# =========================================================== static sub-package
%package static
Summary: %{summary} static version
Group: Applications/Multimedia

%description static
DarkIce  is an IceCast live audio streamer. It takes audio input from a sound
card, encodes it into  mp3, and sends the stream to an IceCast server.

This is the statically linked version 


# =================================================================== prep stage
%prep
%setup


# ================================================================== build stage
%build
# first build the static executable
%configure --enable-static
make all

mv src/darkice src/darkice.static

# there is no need to clean, as the object files are the same for the static
# and dynamic builds
#make distclean

# secondly build the dynamic executable
%configure
make all


# ================================================================ install stage
%install
%makeinstall

# copy the static executable as well
cp src/darkice.static $RPM_BUILD_ROOT/%{_bindir}


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
%doc COPYING ChangeLog README TODO
%config %{_sysconfdir}/darkice.cfg
%{_bindir}/darkice
%{_mandir}/man1/darkice.1*


# ===================================================== static sub-package files
%files static
%defattr (-, root, root)
%doc COPYING ChangeLog README TODO
%config %{_sysconfdir}/darkice.cfg
%{_bindir}/darkice.static
%{_mandir}/man1/darkice.1*
%{_mandir}/man5/darkice.cfg.5*


# =================================================================== change log
#
#   $Log$
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

