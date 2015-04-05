find_package (KdepimLibs REQUIRED)
find_package (Boost REQUIRED)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
find_package(KDReports)

if (KDReports_FOUND)
    include_directories(${KDReports_INCLUDE_DIR} ${KDReports_INCLUDE_DIR}/KDReports)
endif()

include_directories( ${KDEPIMLIBS_INCLUDE_DIRS} ${KDE4_INCLUDES} ${QT_INCLUDES} ${Boost_INCLUDE_DIR} )

add_subdirectory( src )
#add_subdirectory( icons )
