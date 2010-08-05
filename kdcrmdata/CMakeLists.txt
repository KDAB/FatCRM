project(kdcrmdata)

find_package(KDE4 REQUIRED)
include(KDE4Defaults)

find_package(KdepimLibs REQUIRED)
find_package(Boost REQUIRED)

include_directories( ${KDEPIMLIBS_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${QT_INCLUDES} ${Boost_INCLUDE_DIR} )

set(kdcrmdata_LIB_SRCS
    sugaraccount.cpp
    sugaraccountio.cpp
    sugaropportunity.cpp
    sugaropportunityio.cpp
    sugarlead.cpp
    sugarleadio.cpp
    sugarcampaign.cpp
    sugarcampaignio.cpp
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
set(akonadi_serializer_sugaropportunity_PART_SRCS serializerpluginsugaropportunity.cpp)
kde4_add_plugin(akonadi_serializer_sugaropportunity ${akonadi_serializer_sugaropportunity_PART_SRCS})
target_link_libraries(akonadi_serializer_sugaropportunity kdcrmdata ${KDE4_CORE_LIBS} ${KDEPIMLIBS_AKONADI_LIBS})
install (TARGETS akonadi_serializer_sugaropportunity DESTINATION ${PLUGIN_INSTALL_DIR})

### next target
set(akonadi_serializer_sugarlead_PART_SRCS serializerpluginsugarlead.cpp )
kde4_add_plugin(akonadi_serializer_sugarlead ${akonadi_serializer_sugarlead_PART_SRCS})
target_link_libraries(akonadi_serializer_sugarlead kdcrmdata ${KDE4_KDECORE_LIBS} ${KDEPIMLIBS_AKONADI_LIBS})
install(TARGETS akonadi_serializer_sugarlead DESTINATION ${PLUGIN_INSTALL_DIR})

### next target
set(akonadi_serializer_sugarcampaign_PART_SRCS serializerpluginsugarcampaign.cpp )
kde4_add_plugin(akonadi_serializer_sugarcampaign ${akonadi_serializer_sugarcampaign_PART_SRCS})
target_link_libraries(akonadi_serializer_sugarcampaign kdcrmdata ${KDE4_KDECORE_LIBS} ${KDEPIMLIBS_AKONADI_LIBS})
install(TARGETS akonadi_serializer_sugarcampaign DESTINATION ${PLUGIN_INSTALL_DIR})

### next target

#if (KDE4_BUILD_TESTS)
#  add_subdirectory(tests)
#endif (KDE4_BUILD_TESTS)

########### install files ###############

install( FILES
  akonadi_serializer_sugaraccount.desktop
  akonadi_serializer_sugaropportunity.desktop
  akonadi_serializer_sugarlead.desktop
  akonadi_serializer_sugarcampaign.desktop
DESTINATION ${DATA_INSTALL_DIR}/akonadi/plugins/serializer)

