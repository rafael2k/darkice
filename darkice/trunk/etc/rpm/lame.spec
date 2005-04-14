#-------------------------------------------------------------------------------
#
#  Copyright (c) 2000 Tyrell Corporation. All rights reserved.
#
#  Tyrell DarkIce
#
#  File     : lame.spec
#  Version  : $Revision$
#  Author   : $Author$
#  Location : $Source$
#  
#  Abstract : 
#
#   Specification file to build RPM packages of lame.
#   Builds a proper lame executable on a RedHat 7.1 system.
#   Based on the official lame RPM spec file by
#   cefiar <cefiar1@optushome.com.au>
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

# ================================================================= local macros
%define name lame
%define ver 3.91
%define rel 1
%define prefix /usr


# ===================================================================== preamble
Summary : LAME Ain't an MP3 Encoder
Name: %{name}
Version: %{ver}
Release: %{rel}
Copyright: LGPL
Vendor: The LAME Project
Packager: Akos Maroy <darkeye@tyrell.hu>
URL: http://www.mp3dev.org/mp3/
Group: Applications/Multimedia
Source: http://prdownloads.sourceforge.net/darkice/%{name}-%{ver}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{ver}-root
Prefix: %{prefix}
Provides: lame

%description
LAME is an educational tool to be used for learning about MP3 encoding.  The
goal of the LAME project is to use the open source model to improve the
psycho acoustics, noise shaping and speed of MP3.  Another goal of the LAME
project is to use these improvements for the basis of a  patent free audio
compression codec for the GNU project.

This build is optimized for %{_target}.

# ============================================================ devel sub-package
%package devel
Summary: Shared and static libraries for LAME.
Group: Development/Libraries
Requires: %{name} = %{version}

%description devel
LAME is an educational tool to be used for learning about MP3 encoding.
This package contains both the shared and the static libraries from the
LAME project.

You will also need to install the main lame package in order to install
these libraries.

This build is optimized for %{_target}.

# =================================================================== prep stage
%prep
%setup -n %{name}-%{ver}


# ================================================================== build stage
%build
export CC=gcc3
export CFLAGS="-O9"
%configure --with-fileio=lame   \
           --without-vorbis     \
           --disable-gtktest    \
           --enable-nasm        \
           --enable-expopt=full
make


# ================================================================ install stage
%install
%makeinstall


# ======================================================================== clean
%clean
rm -rf $RPM_BUILD_ROOT
make clean


# =========================================================== main package files
%files
%defattr (-,root,root)
%doc LICENSE USAGE COPYING TODO README*
%doc doc/html
%{_bindir}/lame
%{_mandir}/man1/lame.1*
%{_libdir}/libmp3lame.so.*


# ====================================================== devel sub-package files
%files devel
%defattr (-,root,root)
%doc API HACKING STYLEGUIDE
%{_includedir}/lame/lame.h
%{_libdir}/libmp3lame.la
%{_libdir}/libmp3lame.a
%{_libdir}/libmp3lame.so


# =================================================================== change log
#
#   $Log$
#   Revision 1.1  2005/04/14 11:58:08  darkeye
#   moved directory rpm to etc/rpm
#
#   Revision 1.4  2002/02/19 14:52:41  darkeye
#   updated for lame 3.91
#
#   Revision 1.3  2001/09/13 05:06:41  darkeye
#   removed references to SourceForget FTP sites, as they are phased out
#
#   Revision 1.2  2001/09/09 09:06:26  darkeye
#   lame RPM is now created with gcc3 and full optimizations
#
#   Revision 1.1  2001/09/02 12:46:05  darkeye
#   added RPM package creation scripts
#
#

