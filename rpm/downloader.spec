%global repo downloadmanager

%define pkgrelease  1
%if 0%{?openeuler}
%define specrelease %{pkgrelease}
%else
%define specrelease %{pkgrelease}%{?dist}
%endif

Name:           downloadmanager
Version:        5.3.8test4
Release:        1%{pkgrelease}
Summary:        a user-friendly download tool, supporting URLs and torrent files
License:        GPLv3+
URL:            https://github.com/linuxdeepin/org.deepin.downloader
Source0:        %{name}_%{version}.orig.tar.gz

BuildRequires:  gcc-c++
BuildRequires:  cmake
BuildRequires:  qt5-qttools-devel
BuildRequires:  qt5-qtbase-devel
BuildRequires:  qt5-qttools-devel
BuildRequires:  qt5-qtwebchannel-devel
BuildRequires:  qt5-qtwebsockets-devel
BuildRequires:  dtkwidget-devel
BuildRequires:  qt5-qtbase-private-devel
BuildRequires:  gtest-devel
BuildRequires:  gmock-devel
BuildRequires:  qt5-qtsvg-devel
BuildRequires:  libcurl-devel
Requires:       sqlite
Requires:       aria2
Requires:       curl

%description
Downloader is a user-friendly download tool, supporting URLs and torrent files.

%prep
%autosetup -p1 -n %{repo}-%{version}

%build
# help find (and prefer) qt5 utilities, e.g. qmake, lrelease
export PATH=%{_qt5_bindir}:$PATH
%cmake -DCMAKE_BUILD_TYPE=Release
%make_build

%install
%make_install INSTALL_ROOT=%{buildroot}

%files
%doc README.md
%license LICENSE
%{_bindir}/downloader
%{_datadir}/downloader/translations/*.qm
%{_datadir}/applications/downloader.desktop
%{_datadir}/downloader/icons/logo/downloader.svg
%{_datadir}/icons/hicolor/scalable/apps/downloader.svg
%{_datadir}/deepin-manual/manual-assets/application/assets/downloader/*
%{_datadir}/downloader/config/aria2.conf
%{_datadir}/downloader/config/httpAdvanced.json
%{_datadir}/downloader/config/config.conf
%{_datadir}/downloader/config/content-type.conf
%{_datadir}/downloader/database/downloader.db
/usr/lib/libdlmaria2.so
/usr/lib/libdlmcontrolui.so
/usr/lib/libdlmdatabase.so
/usr/lib/libdlmlog.so

%changelog

