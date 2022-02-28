cmake_minimum_required(VERSION 3.16)

# Project & version setup
# 1.x was Qt4/kdelisbs4-based
# 2.x is Qt5/KF5-based
project(FatCRM VERSION 2.5.0)

set(FatCRM_SOVERSION "1.0.0") # for libkdcrmdata

# we need some parts of the ECM CMake helpers
find_package(ECM 5.38.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/ ${ECM_MODULE_PATH})

include(ECMOptionalAddSubdirectory)
include(ECMInstallIcons)
include(ECMSetupVersion)
include(ECMAddTests)
include(ECMMarkNonGuiExecutable)

include(CMakePackageConfigHelpers)
include(FeatureSummary)
include(CheckFunctionExists)
include(GenerateExportHeader)

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMAddAppIcon)
include(ECMQtDeclareLoggingCategory)

enable_testing()

set(QT_REQUIRED_VERSION "5.9.0")
set(KF5_VERSION "5.63.0") # first KF5 release including KF5Contacts & KF5CalendarCore
set(PIM_VERSION "5.19")

find_package(Qt5 ${QT_REQUIRED_VERSION} CONFIG REQUIRED COMPONENTS Test Network Sql PrintSupport)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(KF5Akonadi ${PIM_VERSION} CONFIG REQUIRED)
find_package(KF5AkonadiContact ${PIM_VERSION} CONFIG REQUIRED)
if (KF5AkonadiContact_VERSION VERSION_LESS "5.19.41")
    add_definitions(-DAKONADI_CONTACT_OLD_INCLUDES_LAYOUT)
endif()

find_package(KF5CalendarCore ${KF5_VERSION} CONFIG REQUIRED)
find_package(KF5Contacts ${KF5_VERSION} CONFIG REQUIRED)
find_package(KF5DBusAddons ${KF5_VERSION} CONFIG REQUIRED)
find_package(KF5GuiAddons ${KF5_VERSION} CONFIG REQUIRED)
find_package(KF5I18n ${KF5_VERSION} CONFIG REQUIRED)
find_package(KF5KIO ${KF5_VERSION} CONFIG REQUIRED)
find_package(KF5IconThemes ${KF5_VERSION} CONFIG REQUIRED)
find_package(KF5TextWidgets ${KF5_VERSION} CONFIG REQUIRED)
find_package(KF5WidgetsAddons ${KF5_VERSION} CONFIG REQUIRED)
find_package(KF5WindowSystem ${KF5_VERSION} CONFIG REQUIRED)
find_package(KF5Wallet ${KF5_VERSION} CONFIG REQUIRED)
find_package(KF5ConfigWidgets ${KF5_VERSION} CONFIG REQUIRED)

# KDChart is potentially a dependency of KDReports
# Work-around issue in KDReports CMake config files: It doesn't try to detect the KDChart dependency
#   (can be removed when depending on KDReports 2.1 -- also see: https://github.com/KDAB/KDReports/pull/31)
find_package(KDChart CONFIG QUIET)

find_package(KDReports CONFIG)
set_package_properties(KDReports PROPERTIES
  URL "https://github.com/KDAB/KDReports"
  DESCRIPTION "Qt library for generating printable and exportable reports from code and from XML descriptions"
  TYPE REQUIRED
  PURPOSE "Required to generate reports."
)

find_package(PhoneNumber)
set_package_properties(PhoneNumber PROPERTIES
  URL "https://github.com/googlei18n/libphonenumber"
  DESCRIPTION "Google's common Java, C++ and JavaScript library for parsing, formatting, and validating international phone numbers."
  TYPE OPTIONAL
  PURPOSE "Required to parse phone numbers and extract country names"
)

find_package(ICU COMPONENTS uc)
set_package_properties(ICU PROPERTIES
  URL "http://www.icu-project.org/"
  DESCRIPTION "ICU is a mature, widely used set of C/C++ and Java libraries providing Unicode and Globalization support for software applications."
  TYPE OPTIONAL
  PURPOSE "Required to localize country names"
)

if(PhoneNumber_FOUND AND ICU_FOUND)
    set(USE_PHONENUMBER 1)
else()
    set(USE_PHONENUMBER 0)
endif()
configure_file(config-phonenumber.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-phonenumber.h)

include(GetGitRevisionDescription)
set(FatCRM_EXTENDED_VERSION "${FatCRM_VERSION}")
if(EXISTS "${CMAKE_SOURCE_DIR}/.git")
  git_describe(GIT_DESC --tags)
  if (GIT_DESC)
    set(FatCRM_EXTENDED_VERSION "${FatCRM_EXTENDED_VERSION} (revision: ${GIT_DESC})")
  endif()
endif()

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/config-fatcrm-version.h.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/config-fatcrm-version.h
)

find_package(Xsltproc)
set_package_properties(Xsltproc PROPERTIES DESCRIPTION "XSLT processor from libxslt" TYPE REQUIRED PURPOSE "Required to generate D-Bus interfaces.")

include(CheckIncludeFiles)

# Stolen from zanshin
macro(assert_min_ver version)
  set(error_msg "${CMAKE_CXX_COMPILER} ${CMAKE_CXX_COMPILER_VERSION} not supported")
  if("${CMAKE_CXX_COMPILER_VERSION}" VERSION_LESS "${version}")
    message(FATAL_ERROR "${msg}")
  endif()
endmacro()

kde_enable_exceptions()

set(CMAKE_CXX_STANDARD 11)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  include(CheckCXXCompilerFlag)
  check_cxx_compiler_flag(-Wunused-but-set-variable HAVE_GXX_UNUSED_BUT_SET)
  check_cxx_compiler_flag(-Wlogical-op HAVE_GXX_LOGICAL_OP)
  check_cxx_compiler_flag(-Wsizeof-pointer-memaccess HAVE_GXX_POINTER_MEMACCESS)
  check_cxx_compiler_flag(-Wreorder HAVE_GXX_REORDER)
  check_cxx_compiler_flag(-Wformat-security HAVE_GXX_FORMAT_SECURITY)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden -Wno-deprecated -Wall -Wextra -Woverloaded-virtual -Winit-self -Wunused -Wno-div-by-zero -Wundef -Wpointer-arith -Wmissing-noreturn -Werror=return-type")
  if(HAVE_GXX_UNUSED_BUT_SET)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunused-but-set-variable")
  endif()
  if(HAVE_GXX_LOGICAL_OP)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wlogical-op")
  endif()
  if(HAVE_GXX_POINTER_MEMACCESS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wsizeof-pointer-memaccess")
  endif()
  if(HAVE_GXX_REORDER)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wreorder")
  endif()
  if(HAVE_GXX_FORMAT_SECURITY)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wformat -Wformat-security")
  endif()
  if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND Qt5Core_VERSION VERSION_LESS 5.5)
    # disable warnings about inconsistent usage of 'override' specifier for Qt <5.5
    # only starting with 5.5 Qt started to use 'override' throughout the code base
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-inconsistent-missing-override")
  endif()
endif()
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Qunused-arguments")
endif()

add_subdirectory(kdcrmdata)
add_subdirectory(client)
add_subdirectory(resources)
add_subdirectory(invoker)
add_subdirectory(tests)
install( FILES fatcrm.categories DESTINATION ${KDE_INSTALL_CONFDIR} )

message(STATUS "Building FatCRM ${FatCRM_EXTENDED_VERSION} in ${CMAKE_BUILD_TYPE} mode")
feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
