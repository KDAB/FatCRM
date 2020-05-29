# - Find KDReports
# This module finds if KDReports is installed.
#
# KDReports_FOUND       - Set to TRUE if KDReports was found.
# KDReports_LIBRARIES   - Path to KDReports libraries.
# KDReports_INCLUDE_DIR - Path to the KDReports include directory.
#
# Copyright (C) 2007-2020 Klaralvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

# Redistribution and use is allowed according to the terms of the BSD license

include(FindPackageHandleStandardArgs)

find_library(KDReports_LIBRARIES
    NAMES KDReports kdreports kdreports1 kdreports1d kdreportsd1 NAMES_PER_DIR
    HINTS ${KDReports_DIR} ENV KDReports_DIR
    PATH_SUFFIXES lib)
find_path(KDReports_INCLUDE_DIR
    NAMES KDReportsGlobal.h
    HINTS ${KDReports_DIR} ENV KDReports_DIR
    PATH_SUFFIXES include include/KDReports)

mark_as_advanced(KDReports_LIBRARIES KDReports_INCLUDE_DIR)

find_package_handle_standard_args(KDReports DEFAULT_MSG KDReports_LIBRARIES KDReports_INCLUDE_DIR)
