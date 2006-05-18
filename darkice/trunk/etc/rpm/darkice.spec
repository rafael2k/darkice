%{!?dist: %define dist fc3}

Summary : DarkIce live IceCast / ShoutCast streamer
Name: darkice
Vendor: Tyrell Hungary
Packager: Akos Maroy <darkeye@tyrell.hu>
Version: 0.18
Release: 1.%{dist}
Copyright: GPL
Group: Applications/Multimedia
Source: http://prdownloads.sourceforge.net/darkice/darkice-%{version}.tar.gz
URL: http://darkice.sourceforge.net/
Provides: darkice
BuildRoot: %{_tmppath}/%{name}-%{version}-root
BuildPrereq: lame-devel libogg-devel libvorbis-devel
%if %{dist} != el3
BuildPrereq: jack-audio-connection-kit-devel
%endif
Prefix: /usr

%description
DarkIce is an IceCast, IceCast2 and ShoutCast live audio streamer. It
takes audio input from a sound card, encodes it into mp3 and/or Ogg Vorbis,
and sends the mp3 stream to one or more IceCast and/or ShoutCast servers,
the Ogg Vorbis stream to one or more IceCast2 servers.

%prep
%setup

%build
%configure \
%if %{dist} == el3
 --without-alsa --without-jack
%endif

make all

%install
rm -fr %{buildroot}
%makeinstall

%clean
rm -rf %{buildroot}

%files
%defattr (-, root, root)
%doc COPYING ChangeLog README TODO AUTHORS
%config(noreplace) %{_sysconfdir}/darkice.cfg
%{_bindir}/darkice
%{_mandir}/man1/darkice.1*
%{_mandir}/man5/darkice.cfg.5*


%changelog 
* Thu May 18 2006 Akos Maroy <darkeye@tyrell.hu> 0.18-1
- rewrite the spec file for 0.18
* Tue Apr 19 2005 Akos Maroy <darkeye@tyrell.hu> 0.16-1
- rewrite the spec file for 0.16
* Tue Apr 19 2005 Akos Maroy <darkeye@tyrell.hu> 0.15-1
- rewrite the spec file for 0.15
