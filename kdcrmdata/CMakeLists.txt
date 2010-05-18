project(kdcrmdata)

find_package(KDE4 REQUIRED)
include(KDE4Defaults)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(kdcrmdata_LIB_SRCS
    sugaraccount.cpp
)

kde4_add_library(kdcrmdata SHARED ${kdcrmdata_LIB_SRCS} )

target_link_libraries(kdcrmdata
    ${QT_QTCORE_LIBRARY}
)

set_target_properties(kdcrmdata PROPERTIES VERSION ${GENERIC_LIB_VERSION} SOVERSION ${GENERIC_LIB_SOVERSION})

install(TARGETS kdcrmdata ${INSTALL_TARGETS_DEFAULT_ARGS})

#if (KDE4_BUILD_TESTS)
#  add_subdirectory(tests)
#endif (KDE4_BUILD_TESTS)
