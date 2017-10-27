cmake_minimum_required(VERSION 3.0)
project(FatCRM)

if(POLICY CMP0028)
  cmake_policy(SET CMP0028 NEW) # CMP0028: Double colon in target name means ALIAS or IMPORTED target.
endif()
if(POLICY CMP0063)
  cmake_policy(SET CMP0063 NEW)
endif()
if(POLICY CMP0059)
  cmake_policy(SET CMP0059 OLD) # we use DEFINITIONS as a built-in directory property
endif()

# we need some parts of the ECM CMake helpers
find_package(ECM 5.12.0 REQUIRED NO_MODULE)
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

set(QT_REQUIRED_VERSION "5.4.0")
set(KF5_VERSION "5.12.0")
set(KCONTACTS_LIB_VERSION "4.89.0")
set(KCALENDARCORE_LIB_VERSION "4.81.0")

find_package(Qt5Test ${QT_REQUIRED_VERSION} CONFIG REQUIRED)
find_package(Qt5Sql ${QT_REQUIRED_VERSION} CONFIG REQUIRED)

find_package(KF5DBusAddons ${KF5_VERSION} CONFIG REQUIRED)
find_package(KF5Akonadi "5.1" CONFIG QUIET)
if (NOT KF5Akonadi_FOUND)
    find_package(KF5Akonadi "4.80" CONFIG REQUIRED)
    add_definitions(-DAKONADI_OLD_API)
endif()

find_package(KF5Contacts ${KCONTACTS_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5CalendarCore ${KCALENDARCORE_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5AkonadiContact CONFIG REQUIRED)
find_package(KF5I18n CONFIG REQUIRED)
find_package(KF5IconThemes CONFIG REQUIRED)
find_package(KF5TextWidgets CONFIG REQUIRED)
find_package(KF5KDELibs4Support CONFIG REQUIRED)

# Version setup
# 1.x was Qt4/kdelisbs4-based
# 2.x is Qt5/KF5-based
set(FATCRM_VERSION_MAJOR "2")
set(FATCRM_VERSION_MINOR "0")
set(FATCRM_VERSION_PATCH "0")
set(FATCRM_VERSION "${FATCRM_VERSION_MAJOR}.${FATCRM_VERSION_MINOR}.${FATCRM_VERSION_PATCH}")
set(FATCRM_VERSION_STRING "${FATCRM_VERSION}")
set(FATCRM_SOVERSION "1.0.0") # for libkdcrmdata

include(GetGitRevisionDescription)
if(EXISTS "${CMAKE_SOURCE_DIR}/.git")
  git_describe(GIT_DESC --tags)
  set(FATCRM_VERSION_STRING "${FATCRM_VERSION_STRING} (revision: ${GIT_DESC})")
endif()

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/config-fatcrm-version.h.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/config-fatcrm-version.h
)

# search packages used by KDE
find_package(Boost REQUIRED)

find_package(Xsltproc REQUIRED)
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

if(CMAKE_COMPILER_IS_GNUCXX OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  include(CheckCXXCompilerFlag)
  check_cxx_compiler_flag(-Wunused-but-set-variable HAVE_GXX_UNUSED_BUT_SET)
  check_cxx_compiler_flag(-Wlogical-op HAVE_GXX_LOGICAL_OP)
  check_cxx_compiler_flag(-Wsizeof-pointer-memaccess HAVE_GXX_POINTER_MEMACCESS)
  check_cxx_compiler_flag(-Wreorder HAVE_GXX_REORDER)
  check_cxx_compiler_flag(-Wformat-security HAVE_GXX_FORMAT_SECURITY)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden -std=c++11 -Wno-deprecated -Wall -Wextra -Woverloaded-virtual -Winit-self -Wunused -Wno-div-by-zero -Wundef -Wpointer-arith -Wmissing-noreturn -Werror=return-type")
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
  if(CMAKE_SYSTEM_NAME MATCHES Linux OR CMAKE_SYSTEM_NAME STREQUAL GNU)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_XOPEN_SOURCE=500 -D_DEFAULT_SOURCE -D_GNU_SOURCE")
  endif()
  if(APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-undef") #quiet lots of warnings in Boost
  endif()
  if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND Qt5Core_VERSION VERSION_LESS 5.5)
    # disable warnings about inconsistent usage of 'override' specifier for Qt <5.5
    # only starting with 5.5 Qt started to use 'override' throughout the code base
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-inconsistent-missing-override")
  endif()
endif()
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Qunused-arguments")
endif()

include_directories(
  ${Boost_INCLUDE_DIR}
)

add_subdirectory(kdcrmdata)
add_subdirectory(client)
add_subdirectory(resources)
add_subdirectory(invoker)
add_subdirectory(images)
add_subdirectory(tests)
install( FILES fatcrm.categories DESTINATION ${KDE_INSTALL_CONFDIR} )

message(STATUS "Building FatCRM ${FATCRM_VERSION_STRING} in ${CMAKE_BUILD_TYPE} mode")
feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
