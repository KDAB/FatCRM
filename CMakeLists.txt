project( FatCRM )

cmake_minimum_required(VERSION 2.4)

find_package(KDE4 REQUIRED)
include (KDE4Defaults)

# search packages used by KDE
include(MacroLibrary)
include(MacroOptionalAddSubdirectory)
include(CheckIncludeFiles)

add_subdirectory( kdcrmdata )
add_subdirectory( client )
add_subdirectory( resources )
add_subdirectory( tests )
