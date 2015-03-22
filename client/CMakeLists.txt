find_package (KdepimLibs REQUIRED)
find_package (Boost REQUIRED)

include_directories( ${KDEPIMLIBS_INCLUDE_DIRS} ${KDE4_INCLUDES} ${QT_INCLUDES} ${Boost_INCLUDE_DIR} )

add_subdirectory( src )
#add_subdirectory( icons )
