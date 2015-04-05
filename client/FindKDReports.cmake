# - Find KDReports
# This module finds if KDReports is installed.
#
# KDReports_FOUND		- Set to TRUE if KDReports was found.
# KDReports_LIBRARIES	- Path to KDReports libraries.
# KDReports_INCLUDE_DIR	- Path to the KDReports include directory.
#
# This file was generated automatically.
# Please edit generate-configure.sh rather than this file.

include(FindPackageHandleStandardArgs)

find_library(KDReports_LIBRARIES 
	NAMES KDReports kdreports 
	PATH_SUFFIXES lib bin
   HINTS ENV KDREPORTSDIR)
find_path(KDReports_INCLUDE_DIR 
	NAMES KDReports KDReportsGlobal
   PATH_SUFFIXES include
   HINTS ENV KDREPORTSDIR)

mark_as_advanced(KDReports_LIBRARIES KDReports_INCLUDE_DIR)

find_package_handle_standard_args(KDReports DEFAULT_MSG KDReports_LIBRARIES KDReports_INCLUDE_DIR)

