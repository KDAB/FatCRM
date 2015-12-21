Introduction
============
FatCRM is a desktop application for SugarCRM.

Desktop application for a web service, fat client, get it? :-)

SugarCRM (and its opensource version SuiteCRM) is a web-oriented system for tracking
customers and potential customers. The downside of web user interfaces is the latency
that applies to every action, requiring roundtrips to the web server.

FatCRM solves this by creating a local cache of the data (accounts, opportunities, contacts, etc.)
and providing a desktop application to search, view, and edit this data.

Dependencies
============
FatCRM uses Qt4, KDSoap, KDReports, Akonadi, and kdepimlibs4+kdelibs4.

Compiling from sources, on Linux
================================
* Prerequisites

Install the package for kdepimlibs development based on Qt4: libkdepimlibs4-devel (OpenSuSE), kdepimlib5-dev (Debian/Ubuntu).

* Install KDSoap

```
git clone https://github.com/KDAB/KDSoap.git
cd KDSoap
cmake . -DKDSoap_ENFORCE_QT4_BUILD=true -DCMAKE_INSTALL_PREFIX=/usr/local/KDAB/KDSoap-Qt4
make
sudo make install
cd ..
```

* Install KDReports

```
git clone https://github.com/KDAB/KDReports.git
cd KDReports
./autogen.py -prefix /usr/local/KDAB/KDReports-Qt4
make
sudo make install
cd ..
```

* Compile FatCRM

```
git clone https://github.com/KDAB/FatCRM
cd FatCRM ; mkdir build ; cd build
export CMAKE_PREFIX_PATH=/usr/local/KDAB/KDSoap-Qt4:/usr/local/KDAB/KDReports-Qt4:$CMAKE_PREFIX_PATH
cmake .. -DCMAKE_INSTALL_PREFIX=`kde4-config --prefix`
make
sudo make install
```

* Web settings

In SugarCRM, click on your own name in the topright corner, go to the Advanced tab, and in the first group ("User Settings"), ensure that "Show Full Names" is checked.

* Starting FatCRM

The executable name is fatcrm. It should appear in your K menu under the name "FatCRM".

* First time setup

The first time, you'll have to configure your SugarCRM account. Click on "Add...", type Sugar, click on "SugarCRM Resource".

After that, go to the Settings dialog and configure your full user name at the top, as shown in the web interface for SugarCRM.

Contributing
============
KDAB will happily accept external contributions, but substantial
contributions will require a signed Copyright Assignment Agreement.
Contact support@kdab.com for more information.

License
=======
The FatCRM Software is (C) 2010-2015 Klarälvdalens Datakonsult AB (KDAB),
and is available under the terms of the GPL version 2 (or any later version,
at your option).  See LICENSE.GPL.txt for license details.

A commercial license can be negotiated, contact KDAB for this.

About KDAB
==========
FatCRM is supported and maintained by Klarälvdalens Datakonsult AB (KDAB).

KDAB, the Qt experts, provide consulting and mentoring for developing
Qt applications from scratch and in porting from all popular and legacy
frameworks to Qt. We continue to help develop parts of Qt and are one
of the major contributors to the Qt Project. We can give advanced or
standard trainings anywhere around the globe.

Please visit http://www.kdab.com to meet the people who write code like this.

