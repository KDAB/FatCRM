# - Find PhoneNumber
# This module finds if PhoneNumber is installed.
#
# PhoneNumber_FOUND                 - Set to TRUE if PhoneNumber was found.
# PhoneNumber_LIBRARIES             - Path to PhoneNumber libraries.
# PhoneNumber_INCLUDE_DIR           - Path to the PhoneNumber include directory.
# PhoneNumberGeoCoding_LIBRARIES    - Path to PhoneNumber GeoCodeing libraries.
#
# Copyright (C) 2017 Klaralvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

# Redistribution and use is allowed according to the terms of the BSD license

include(FindPackageHandleStandardArgs)

find_library(PhoneNumber_LIBRARIES
        NAMES phonenumber
        PATH_SUFFIXES lib
        HINTS ENV PHONENUMBERDIR)

find_path(PhoneNumber_INCLUDE_DIR
        NAMES phonenumbers/phonenumberutil.h
        HINTS ENV PHONENUMBERDIR)

find_library(PhoneNumberGeoCoding_LIBRARIES
        NAMES geocoding
        PATH_SUFFIXES lib
        HINTS ENV PHONENUMBERDIR)

mark_as_advanced(PhoneNumber_LIBRARIES PhoneNumber_INCLUDE_DIR)
mark_as_advanced(PhoneNumberGeoCoding_LIBRARIES)

find_package_handle_standard_args(PhoneNumber DEFAULT_MSG PhoneNumber_LIBRARIES PhoneNumber_INCLUDE_DIR PhoneNumberGeoCoding_LIBRARIES)
