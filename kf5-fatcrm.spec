Name:           kf5-fatcrm
Version:        1.3.2
Release:        0
Summary:        desktop application for SugarCRM
Source:         %{name}-%{version}.tar.gz
Url:            https://github.com/KDAB/FatCRM
Group:          Productivity/Other
License:        GPL-2.0+
BuildRoot:      %{_tmppath}/%{name}-%{version}-build
Vendor:         Klaralvdalens Datakonsult AB (KDAB)
Packager:       Klaralvdalens Datakonsult AB (KDAB) <info@kdab.com>

BuildRequires: qt5-kdsoap-devel qt5-kdreports-devel
%if %{defined suse_version}
BuildRequires:  python libqt5-qtbase-devel cmake update-desktop-files
%endif

%if %{defined fedora}
BuildRequires:  gcc-c++ extra-cmake-modules python qt5-qtbase-devel cmake desktop-file-utils kf5-kwallet-devel kf5-kcontacts-devel kf5-akonadi-server-devel kf5-kcalendarcore-devel kf5-akonadi-contacts-devel
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
     David Faure <david.faure@kdab.com>

%prep
%setup -T -c
%{__tar} -zxf %{SOURCE0} --strip-components=1

%build
cmake . -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Release
%__make %{?_smp_mflags}

%install
%make_install

%if %{defined suse_version}
%suse_update_desktop_file fatcrm Utility DesktopUtility
%suse_update_desktop_file fatcrminvoker Utility DesktopUtility
%endif

%clean
%__rm -rf "%{buildroot}"

%files
%defattr(-,root,root)
%{_sysconfdir}/xdg/fatcrm.categories
%{_libdir}/libkdcrmdata.*
%{_libdir}/libfatcrmprivate.*
%{_libdir}/plugins/akonadi_serializer_*
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
* Wed Nov 09 2016 Allen Winter <allen.winter@kdab.com> 1.3.2
- 1.3.2 pre-release
