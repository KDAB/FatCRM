project(kdcrmdata)

find_package(KDE4 REQUIRED)
include(KDE4Defaults)

find_package(KdepimLibs REQUIRED)

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}${QT_INCLUDES} )

set(kdcrmdata_LIB_SRCS
    sugaraccount.cpp
    sugaraccountio.cpp
)

kde4_add_library(kdcrmdata SHARED ${kdcrmdata_LIB_SRCS} )

target_link_libraries(kdcrmdata
    ${QT_QTCORE_LIBRARY}
    ${KDEPIMLIBS_AKONADI_LIBS}
)

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${KDE4_ENABLE_EXCEPTIONS}" )

set_target_properties(kdcrmdata PROPERTIES VERSION ${GENERIC_LIB_VERSION} SOVERSION ${GENERIC_LIB_SOVERSION})

install(TARGETS kdcrmdata ${INSTALL_TARGETS_DEFAULT_ARGS})

### next target

set(akonadi_serializer_sugaraccount_PART_SRCS serializerpluginsugaraccount.cpp )
kde4_add_plugin(akonadi_serializer_sugaraccount ${akonadi_serializer_sugaraccount_PART_SRCS})
target_link_libraries(akonadi_serializer_sugaraccount kdcrmdata ${KDE4_KDECORE_LIBS} ${KDEPIMLIBS_AKONADI_LIBS})
install(TARGETS akonadi_serializer_sugaraccount DESTINATION ${PLUGIN_INSTALL_DIR})

### next target

#if (KDE4_BUILD_TESTS)
#  add_subdirectory(tests)
#endif (KDE4_BUILD_TESTS)
