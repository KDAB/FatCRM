project(kdcrmdata)

find_package(KDE4 REQUIRED)
include(KDE4Defaults)

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}${QT_INCLUDES} )

set(kdcrmdata_LIB_SRCS
    sugaraccount.cpp
    sugaraccountio.cpp
    serializerpluginsugaraccount.cpp 
)

kde4_add_library(kdcrmdata SHARED ${kdcrmdata_LIB_SRCS} )

target_link_libraries(kdcrmdata
    ${QT_QTCORE_LIBRARY}
    ${KDE4_AKONADI_LIBS}
)

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${KDE4_ENABLE_EXCEPTIONS}" )

set_target_properties(kdcrmdata PROPERTIES VERSION ${GENERIC_LIB_VERSION} SOVERSION ${GENERIC_LIB_SOVERSION})

install(TARGETS kdcrmdata ${INSTALL_TARGETS_DEFAULT_ARGS})

#if (KDE4_BUILD_TESTS)
#  add_subdirectory(tests)
#endif (KDE4_BUILD_TESTS)
