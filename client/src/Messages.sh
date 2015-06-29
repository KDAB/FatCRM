#! /usr/bin/env bash
$EXTRACTRC `find . -name \*.rc` >> rc.cpp
$XGETTEXT */*.cpp -o $podir/fatcrm.pot
rm -f rc.cpp
