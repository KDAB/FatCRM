find_package (KdepimLibs REQUIRED)
find_package (Boost REQUIRED)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
find_package(KDReports REQUIRED)
set_package_properties(KDReports PROPERTIES URL "https://github.com/KDAB/KDReports" DESCRIPTION "Qt library for generating printable and exportable reports from code and from XML descriptions" TYPE REQUIRED PURPOSE "Required to generate reports.")

include_directories(${KDReports_INCLUDE_DIR} ${KDReports_INCLUDE_DIR}/KDReports)

find_package(Xsltproc)
set_package_properties(Xsltproc PROPERTIES DESCRIPTION "XSLT processor from libxslt" TYPE REQUIRED PURPOSE "Required to generate D-Bus interfaces.")

include_directories( ${KDEPIMLIBS_INCLUDE_DIRS} ${KDE4_INCLUDES} ${QT_INCLUDES} ${Boost_INCLUDE_DIR} )

add_subdirectory( src )
add_subdirectory( icons )
