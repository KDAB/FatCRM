Name:           fatcrm
Version:        2.4.0
Release:        0
Summary:        Desktop application for SugarCRM
Source0:        %{name}-%{version}.tar.gz
Source1:        %{name}-%{version}.tar.gz.asc
Url:            https://github.com/KDAB/FatCRM
Group:          Productivity/Other
License:        GPL-2.0-or-later
BuildRoot:      %{_tmppath}/%{name}-%{version}-build
Vendor:         Klaralvdalens Datakonsult AB (KDAB)
Packager:       Klaralvdalens Datakonsult AB (KDAB) <info@kdab.com>

BuildRequires: cmake extra-cmake-modules kdsoap-devel qt5-kdreports-devel
%if %{defined suse_version}
Requires: akonadi-server
BuildRequires:  python libxslt-tools libqt5-qtbase-devel update-desktop-files kdbusaddons-devel akonadi-server-devel kio-devel kwindowsystem-devel ktextwidgets-devel kguiaddons-devel ki18n-devel kiconthemes-devel kconfigwidgets-devel kdbusaddons-devel kcalcore-devel kcontacts-devel akonadi-contact-devel akonadi-calendar-devel kwallet-devel libicu-devel
%endif

%if %{defined fedora}
Requires: akonadi
BuildRequires:  gcc-c++ shared-mime-info python qt5-qtbase-devel desktop-file-utils kf5-kio-devel kf5-kwindowsystem-devel kf5-ktextwidgets-devel kf5-kguiaddons-devel kf5-ki18n-devel kf5-kiconthemes-devel kf5-kconfigwidgets-devel kf5-kdbusaddons-devel kf5-kwallet-devel kf5-kcontacts-devel kf5-akonadi-server-devel kf5-kcalendarcore-devel kf5-akonadi-contacts-devel generic-logos
%endif

%description
FatCRM is a desktop application for SugarCRM.

SugarCRM (and its opensource version SuiteCRM) is a web-oriented system for tracking
customers and potential customers. The downside of web user interfaces is the latency
that applies to every action, requiring roundtrips to the web server.

FatCRM solves this by creating a local cache of the data (accounts, opportunities,
contacts, etc.) and providing a desktop application to search, view, and edit this data.

Authors:
--------
     Klaralvdalens Datakonsult AB (KDAB) <info@kdab.com>

%define debug_package %{nil}
%global __debug_install_post %{nil}

%prep
%setup -T -c
%{__tar} -zxf %{SOURCE0} --strip-components=1

%build
cmake . -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Release
%__make %{?_smp_mflags}

%install
%make_install

%if %{defined suse_version}
%suse_update_desktop_file -c org.kde.fatcrm FatCRM "Desktop application for SugarCRM" fatcrm fatcrm Utility DesktopUtility
%suse_update_desktop_file fatcrminvoker Utility DesktopUtility
%endif

%post -p /sbin/ldconfig
%postun -p /sbin/ldconfig

%clean
%__rm -rf "%{buildroot}"

%files
%defattr(-,root,root)
%config %{_sysconfdir}/xdg/fatcrm.categories
%{_libdir}/libkdcrmdata.*
%{_libdir}/libfatcrmprivate.*
%if %{defined suse_version}
%dir %{_prefix}/share/akonadi/plugins/
%dir %{_prefix}/share/akonadi/plugins/serializer
%{_libdir}/qt5/plugins/akonadi_serializer_*
%else
%if 0%{?fedora} > 28
%{_libdir}/qt5/plugins/akonadi_serializer_*
%else
%{_libdir}/plugins/akonadi_serializer_*
%endif
%endif
%{_prefix}/share/akonadi/plugins/serializer/akonadi_serializer_*.desktop
%{_prefix}/share/mime/packages/kdabcrm-mime.xml
%{_prefix}/bin/fatcrm
%{_prefix}/bin/fatcrminvoker
%{_prefix}/bin/akonadi_salesforce_resource
%{_prefix}/bin/akonadi_sugarcrm_resource
%{_prefix}/share/applications/org.kde.fatcrm.desktop
%{_prefix}/share/applications/fatcrminvoker.desktop
%{_prefix}/share/icons/hicolor
%dir %{_prefix}/share/akonadi/
%dir %{_prefix}/share/akonadi/agents/
%{_prefix}/share/akonadi/agents/salesforceresource.desktop
%{_prefix}/share/akonadi/agents/sugarcrmresource.desktop

%changelog
* Thu Dec 09 2021 Allen Winter <allen.winter@kdab.com> 2.4.0
- 2.4.0 release
* Tue Dec 08 2020 Allen Winter <allen.winter@kdab.com> 2.3.2
- 2.3.2 release
* Mon Dec 07 2020 Allen Winter <allen.winter@kdab.com> 2.3.1
- 2.3.1 release
* Wed Sep 16 2020 Allen Winter <allen.winter@kdab.com> 2.3.0
- 2.3.0 release
* Sun Jun 24 2018 Allen Winter <allen.winter@kdab.com> 2.1.0
- 2.1.0 release
* Wed Nov 09 2016 Allen Winter <allen.winter@kdab.com> 1.3.2
- 1.3.2 pre-release
