project(sugarclient)

find_package(KDE4 REQUIRED)
include (KDE4Defaults)

find_package (KdepimLibs REQUIRED)

include_directories( ${KDEPIMLIBS_INCLUDE_DIRS} ${KDE4_INCLUDES} ${QT_INCLUDES} )

add_subdirectory( src )
#add_subdirectory( icons )
