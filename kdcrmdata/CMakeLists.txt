set(SHARED_MIME_INFO_MINIMUM_VERSION "0.20")
find_package(SharedMimeInfo REQUIRED)

set(kdcrmdata_LIB_SRCS
  kdcrmutils.cpp
  kdcrmfields.cpp
  sugaraccount.cpp
  sugaraccountio.cpp
  sugaropportunity.cpp
  sugaropportunityio.cpp
  sugarlead.cpp
  sugarleadio.cpp
  sugarcampaign.cpp
  sugarcampaignio.cpp
  sugaremail.cpp
  sugaremailio.cpp
  sugarnote.cpp
  sugarnoteio.cpp
)

kde4_add_library(kdcrmdata SHARED ${kdcrmdata_LIB_SRCS})

target_link_libraries(kdcrmdata PUBLIC
  ${QT_QTCORE_LIBRARY}
  ${KDEPIMLIBS_AKONADI_LIBS}
)

set_target_properties(kdcrmdata PROPERTIES
  VERSION ${GENERIC_LIB_VERSION}
  SOVERSION ${GENERIC_LIB_SOVERSION}
)

target_include_directories(kdcrmdata PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>")

install(TARGETS kdcrmdata ${INSTALL_TARGETS_DEFAULT_ARGS})

### next target
set(akonadi_serializer_sugaraccount_PART_SRCS serializerpluginsugaraccount.cpp)
kde4_add_plugin(akonadi_serializer_sugaraccount
  ${akonadi_serializer_sugaraccount_PART_SRCS}
)
target_link_libraries(akonadi_serializer_sugaraccount
  kdcrmdata
  ${KDE4_KDECORE_LIBS}
  ${KDEPIMLIBS_AKONADI_LIBS}
)
install(TARGETS akonadi_serializer_sugaraccount DESTINATION ${PLUGIN_INSTALL_DIR})

### next target
set(akonadi_serializer_sugaropportunity_PART_SRCS serializerpluginsugaropportunity.cpp)
kde4_add_plugin(akonadi_serializer_sugaropportunity
  ${akonadi_serializer_sugaropportunity_PART_SRCS}
)
target_link_libraries(akonadi_serializer_sugaropportunity
  kdcrmdata
  ${KDE4_CORE_LIBS}
  ${KDEPIMLIBS_AKONADI_LIBS}
)
install(TARGETS akonadi_serializer_sugaropportunity DESTINATION ${PLUGIN_INSTALL_DIR})

### next target
set(akonadi_serializer_sugarlead_PART_SRCS serializerpluginsugarlead.cpp)
kde4_add_plugin(akonadi_serializer_sugarlead
  ${akonadi_serializer_sugarlead_PART_SRCS}
)
target_link_libraries(akonadi_serializer_sugarlead
  kdcrmdata
  ${KDE4_KDECORE_LIBS}
  ${KDEPIMLIBS_AKONADI_LIBS}
)
install(TARGETS akonadi_serializer_sugarlead DESTINATION ${PLUGIN_INSTALL_DIR})

### next target
set(akonadi_serializer_sugarcampaign_PART_SRCS serializerpluginsugarcampaign.cpp)
kde4_add_plugin(akonadi_serializer_sugarcampaign
  ${akonadi_serializer_sugarcampaign_PART_SRCS}
)
target_link_libraries(akonadi_serializer_sugarcampaign
  kdcrmdata
  ${KDE4_KDECORE_LIBS}
  ${KDEPIMLIBS_AKONADI_LIBS}
)
install(TARGETS akonadi_serializer_sugarcampaign DESTINATION ${PLUGIN_INSTALL_DIR})

### next target
set(akonadi_serializer_sugarnote_PART_SRCS serializerpluginsugarnote.cpp)
kde4_add_plugin(akonadi_serializer_sugarnote
  ${akonadi_serializer_sugarnote_PART_SRCS}
)
target_link_libraries(akonadi_serializer_sugarnote
  kdcrmdata
  ${KDE4_KDECORE_LIBS}
  ${KDEPIMLIBS_AKONADI_LIBS}
)
install(TARGETS akonadi_serializer_sugarnote DESTINATION ${PLUGIN_INSTALL_DIR})

### next target
set(akonadi_serializer_sugaremail_PART_SRCS serializerpluginsugaremail.cpp)
kde4_add_plugin(akonadi_serializer_sugaremail
  ${akonadi_serializer_sugaremail_PART_SRCS}
)
target_link_libraries(akonadi_serializer_sugaremail
  kdcrmdata
  ${KDE4_KDECORE_LIBS}
  ${KDEPIMLIBS_AKONADI_LIBS}
)
install(TARGETS akonadi_serializer_sugaremail DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install(FILES
  akonadi_serializer_sugaraccount.desktop
  akonadi_serializer_sugaropportunity.desktop
  akonadi_serializer_sugarlead.desktop
  akonadi_serializer_sugarcampaign.desktop
  akonadi_serializer_sugarnote.desktop
  akonadi_serializer_sugaremail.desktop
  DESTINATION ${DATA_INSTALL_DIR}/akonadi/plugins/serializer
)

install(FILES kdabcrm-mime.xml DESTINATION ${XDG_MIME_INSTALL_DIR})
update_xdg_mimetypes(${XDG_MIME_INSTALL_DIR})
