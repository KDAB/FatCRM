project(FatCRM)

cmake_minimum_required(VERSION 2.4)
if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif()

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
endif()
#TODO: support a real debugfull, like in KDE
string(TOLOWER ${CMAKE_BUILD_TYPE} _lower)
if(${_lower} MATCHES "debugfull")
  set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "" FORCE)
endif()

enable_testing()

# Version setup
set(FATCRM_VERSION_MAJOR "1")
set(FATCRM_VERSION_MINOR "1")
set(FATCRM_VERSION_PATCH "1")
set(FATCRM_VERSION "${FATCRM_VERSION_MAJOR}.${FATCRM_VERSION_MINOR}.${FATCRM_VERSION_PATCH}")
set(FATCRM_VERSION_STRING "${FATCRM_VERSION}")
set(FATCRM_SOVERSION "0.1.0") # for libkdcrmdata

if(EXISTS "${CMAKE_SOURCE_DIR}/.git")
  find_package(Git)
  #TODO: FeatureSummary requires modern CMake
  #include(FeatureSummary)
  #set_package_properties(Git PROPERTIES TYPE OPTIONAL PURPOSE "Determine exact build version.")
  if(GIT_FOUND)
    execute_process(
      COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
      OUTPUT_VARIABLE _git_revision
    )
    string(REGEX REPLACE "\n" "" _git_revision "${_git_revision}")
    set(FATCRM_VERSION_STRING "${FATCRM_VERSION_STRING} (revision: ${_git_revision})")
  endif()
endif()

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/config-fatcrm-version.h.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/config-fatcrm-version.h
)

# search packages used by KDE
find_package(KDE4 REQUIRED)
include(KDE4Defaults)
find_package(KdepimLibs REQUIRED)
find_package(Boost REQUIRED)

find_package(Xsltproc REQUIRED)
set_package_properties(Xsltproc PROPERTIES DESCRIPTION "XSLT processor from libxslt" TYPE REQUIRED PURPOSE "Required to generate D-Bus interfaces.")

include(MacroLibrary)
include(MacroOptionalAddSubdirectory)
include(CheckIncludeFiles)

# Stolen from zanshin
macro(assert_min_ver version)
  set(error_msg "${CMAKE_CXX_COMPILER} ${CMAKE_CXX_COMPILER_VERSION} not supported")
  if("${CMAKE_CXX_COMPILER_VERSION}" VERSION_LESS "${version}")
    message(FATAL_ERROR "${msg}")
  endif()
endmacro()

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  # using Clang >= 3.3
  assert_min_ver("3.3")
  add_definitions(-DQ_DECL_OVERRIDE=override)
  add_definitions(-DQ_DECL_FINAL=final)
  add_definitions(-DQ_NULLPTR=nullptr)
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  # using GCC >= 4.8
  assert_min_ver("4.8")
  add_definitions(-DQ_DECL_OVERRIDE=override)
  add_definitions(-DQ_DECL_FINAL=final)
  add_definitions(-DQ_NULLPTR=nullptr)
else()
  add_definitions(-DQ_DECL_OVERRIDE=)
  add_definitions(-DQ_DECL_FINAL=)
  add_definitions(-DQ_NULLPTR=0)
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${KDE4_ENABLE_EXCEPTIONS}")

if(CMAKE_COMPILER_IS_GNUCXX OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  include(CheckCXXCompilerFlag)
  check_cxx_compiler_flag(-Wunused-but-set-variable HAVE_GXX_UNUSED_BUT_SET)
  check_cxx_compiler_flag(-Wlogical-op HAVE_GXX_LOGICAL_OP)
  check_cxx_compiler_flag(-Wsizeof-pointer-memaccess HAVE_GXX_POINTER_MEMACCESS)
  check_cxx_compiler_flag(-Wreorder HAVE_GXX_REORDER)
  check_cxx_compiler_flag(-Wformat-security HAVE_GXX_FORMAT_SECURITY)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden -std=c++11 -Wno-deprecated -Wall -Wextra -Woverloaded-virtual -Winit-self -Wmissing-include-dirs -Wunused -Wno-div-by-zero -Wundef -Wpointer-arith -Wmissing-noreturn -Werror=return-type")
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
endif()
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Qunused-arguments")
endif()

include_directories(
  ${QT_INCLUDES}
  ${KDE4_INCLUDES}
  ${KDEPIMLIBS_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIR}
)

add_subdirectory(kdcrmdata)
add_subdirectory(client)
add_subdirectory(resources)
add_subdirectory(tests)

message(STATUS "Building FatCRM ${FATCRM_VERSION_STRING} in ${CMAKE_BUILD_TYPE} mode")
