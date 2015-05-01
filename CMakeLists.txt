project( FatCRM )

cmake_minimum_required(VERSION 2.4)

find_package(KDE4 REQUIRED)
include (KDE4Defaults)

# search packages used by KDE
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
if(UNIX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

add_subdirectory( kdcrmdata )
add_subdirectory( client )
add_subdirectory( resources )
add_subdirectory( tests )
