project(FatCRM)

cmake_minimum_required(VERSION 2.4)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# search packages used by KDE
find_package(KDE4 REQUIRED)
include(KDE4Defaults)
find_package(KdepimLibs REQUIRED)
find_package(Boost REQUIRED)

find_package(Xsltproc)
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
  add_definitions(-Qunused-arguments)
  add_definitions(-DQ_DECL_OVERRIDE=override)
  add_definitions(-DQ_DECL_FINAL=final)
  add_definitions(-DQ_NULLPTR=nullptr)
  add_definitions(-Wno-deprecated-declarations) # akonadi/item.h uses auto_ptr
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
if(UNIX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${KDE4_ENABLE_EXCEPTIONS}")

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
