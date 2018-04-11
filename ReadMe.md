# Introduction

FatCRM is a desktop application for the web application [SugarCRM](https://www.sugarcrm.com).

Desktop application for a web service, fat client, get it? :-)

SugarCRM (and its opensource version SuiteCRM) is a web-oriented system for tracking
customers and potential customers. The downside of web user interfaces is the latency
that applies to every action, requiring roundtrips to the web server.

FatCRM solves this by creating a local cache of the data (accounts, opportunities, contacts, etc.)
and providing a desktop application to search, view, and edit this data.

# Dependencies

FatCRM uses Qt5, KDSoap, KDReports, Akonadi, and KF5.

# Installation

## Linux

For some of the more popular Linux distributions we provide installable packages using the [openSUSE Build Service](https://build.opensuse.org/project/show/isv:KDAB).

## Installer for Windows

Get it here: https://ci.appveyor.com/project/KDAB/fatcrm

# Compiling from Sources

## All Platforms: Using KDE's Craft

Craft is an open source meta build system and package manager. It manages dependencies and builds libraries and applications from source, on *Windows*, *Mac*, *Linux* and *FreeBSD*.

### Setting up Craft

Read through: https://community.kde.org/Craft

### Compiling FatCRM (and dependencies)

For compiling FatCRM:
```
craft --target master -i craft
craft fatcrm
```

### Optional: Creating an installer

```
craft --package fatcrm
```

## Unix: Manual instructions

### Prerequisites

Install the packages:

- for OpenSuse: `kcalcore-devel akonadi-contact-devel`
- for Debian/Kubuntu: `libkf5akonadicontact-dev extra-cmake-modules`

### Install KDSoap (version 1.6.0 or higher)

```
git clone https://github.com/KDAB/KDSoap.git
cd KDSoap
cmake . -DCMAKE_INSTALL_PREFIX=/usr/local/KDAB/KDSoap-Qt5
make
sudo make install
cd ..
```

### Install KDReports

```
git clone https://github.com/KDAB/KDReports.git
cd KDReports
cmake . -DCMAKE_INSTALL_PREFIX=/usr/local/KDAB/KDReports-Qt5
make
sudo make install
cd ..
```

### Compile FatCRM

```
git clone https://github.com/KDAB/FatCRM
cd FatCRM ; mkdir build ; cd build
export CMAKE_PREFIX_PATH=/usr/local/KDAB/KDSoap-Qt5:/usr/local/KDAB/KDReports-Qt5:$CMAKE_PREFIX_PATH
cmake .. -DCMAKE_INSTALL_PREFIX=`kf5-config --prefix`
make
sudo make install
```

Now restart akonadi so that it sees the newly installed resource (on the command-line: `akonadictl restart`)

# Using FatCRM

## Web settings

In SugarCRM, click on your own name in the topright corner, go to the Advanced tab, and in the first group ("User Settings"), ensure that "Show Full Names" is checked.
Further below in the "Locale Settings" group, ensure that the decimal separator is "." (dot) and that the thousands separator is "," (comma).

## Starting FatCRM

The executable name is `fatcrm`. It should appear in your K menu under the name *FatCRM*.

## First time setup

The first time, you'll have to configure your SugarCRM account. Click on *Add...*, type Sugar, click on *SugarCRM Resource*.

After that, go to the Settings dialog and configure your full user name at the top, as shown in the web interface for SugarCRM.

# Contributing

KDAB will happily accept external contributions, but substantial
contributions will require a signed Copyright Assignment Agreement.
Contact support@kdab.com for more information.

# License

The FatCRM Software is (C) 2010-2017 Klarälvdalens Datakonsult AB (KDAB),
and is available under the terms of the GPL version 2 (or any later version,
at your option).  See LICENSE.GPL.txt for license details.

A commercial license can be negotiated, contact KDAB for this.

# About KDAB

FatCRM is supported and maintained by Klarälvdalens Datakonsult AB (KDAB).

KDAB, the Qt experts, provide consulting and mentoring for developing
Qt applications from scratch and in porting from all popular and legacy
frameworks to Qt. We continue to help develop parts of Qt and are one
of the major contributors to the Qt Project. We can give advanced or
standard trainings anywhere around the globe.

Please visit http://www.kdab.com to meet the people who write code like this.
