set(SHARED_MIME_INFO_MINIMUM_VERSION "0.20")
find_package(SharedMimeInfo REQUIRED)

set(kdcrmdata_LIB_SRCS
  enumdefinitionattribute.cpp
  enumdefinitions.cpp
  kdcrmutils.cpp
  kdcrmfields.cpp
  sugaraccount.cpp
  sugaraccountio.cpp
  sugardocument.cpp
  sugardocumentio.cpp
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

add_library(kdcrmdata SHARED ${kdcrmdata_LIB_SRCS})
generate_export_header(kdcrmdata BASE_NAME kdcrmdata)

target_link_libraries(kdcrmdata PUBLIC
  Qt5::Core
  Qt5::DBus
  KF5::AkonadiCore
  KF5::I18n
  KF5::ConfigCore
  KF5::Codecs
  KF5::IconThemes
  KF5::WidgetsAddons
  KF5::TextWidgets
)

set_target_properties(kdcrmdata PROPERTIES
  VERSION ${FatCRM_SOVERSION}
  SOVERSION ${FatCRM_SOVERSION}
)

target_include_directories(kdcrmdata PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>")

install(TARGETS kdcrmdata ${INSTALL_TARGETS_DEFAULT_ARGS})

### next target
set(akonadi_serializer_sugaraccount_PART_SRCS serializerpluginsugaraccount.cpp)
add_library(akonadi_serializer_sugaraccount
 MODULE
  ${akonadi_serializer_sugaraccount_PART_SRCS}
)
target_link_libraries(akonadi_serializer_sugaraccount
  kdcrmdata
  KF5::AkonadiCore
)
install(TARGETS akonadi_serializer_sugaraccount DESTINATION ${PLUGIN_INSTALL_DIR})

### next target
set(akonadi_serializer_sugaropportunity_PART_SRCS serializerpluginsugaropportunity.cpp)
add_library(akonadi_serializer_sugaropportunity
 MODULE
  ${akonadi_serializer_sugaropportunity_PART_SRCS}
)
target_link_libraries(akonadi_serializer_sugaropportunity
  kdcrmdata
  KF5::AkonadiCore
)
install(TARGETS akonadi_serializer_sugaropportunity DESTINATION ${PLUGIN_INSTALL_DIR})

### next target
set(akonadi_serializer_sugarlead_PART_SRCS serializerpluginsugarlead.cpp)
add_library(akonadi_serializer_sugarlead
 MODULE
  ${akonadi_serializer_sugarlead_PART_SRCS}
)
target_link_libraries(akonadi_serializer_sugarlead
  kdcrmdata
  KF5::AkonadiCore
)
install(TARGETS akonadi_serializer_sugarlead DESTINATION ${PLUGIN_INSTALL_DIR})

### next target
set(akonadi_serializer_sugarcampaign_PART_SRCS serializerpluginsugarcampaign.cpp)
add_library(akonadi_serializer_sugarcampaign
 MODULE
  ${akonadi_serializer_sugarcampaign_PART_SRCS}
)
target_link_libraries(akonadi_serializer_sugarcampaign
  kdcrmdata
  KF5::AkonadiCore
)
install(TARGETS akonadi_serializer_sugarcampaign DESTINATION ${PLUGIN_INSTALL_DIR})

### next target
set(akonadi_serializer_sugarnote_PART_SRCS serializerpluginsugarnote.cpp)
add_library(akonadi_serializer_sugarnote
 MODULE
  ${akonadi_serializer_sugarnote_PART_SRCS}
)
target_link_libraries(akonadi_serializer_sugarnote
  kdcrmdata
  KF5::AkonadiCore
)
install(TARGETS akonadi_serializer_sugarnote DESTINATION ${PLUGIN_INSTALL_DIR})

### next target
set(akonadi_serializer_sugaremail_PART_SRCS serializerpluginsugaremail.cpp)
add_library(akonadi_serializer_sugaremail
 MODULE
  ${akonadi_serializer_sugaremail_PART_SRCS}
)
target_link_libraries(akonadi_serializer_sugaremail
  kdcrmdata
  KF5::AkonadiCore
)
install(TARGETS akonadi_serializer_sugaremail DESTINATION ${PLUGIN_INSTALL_DIR})

### next target
set(akonadi_serializer_sugardocument_PART_SRCS serializerpluginsugardocument.cpp)
add_library(akonadi_serializer_sugardocument MODULE
  ${akonadi_serializer_sugardocument_PART_SRCS}
)
target_link_libraries(akonadi_serializer_sugardocument
  kdcrmdata
  KF5::AkonadiCore
)
install(TARGETS akonadi_serializer_sugardocument DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install(FILES
  akonadi_serializer_sugaraccount.desktop
  akonadi_serializer_sugaropportunity.desktop
  akonadi_serializer_sugarlead.desktop
  akonadi_serializer_sugarcampaign.desktop
  akonadi_serializer_sugarnote.desktop
  akonadi_serializer_sugaremail.desktop
  akonadi_serializer_sugardocument.desktop
  DESTINATION ${DATA_INSTALL_DIR}/akonadi/plugins/serializer
)

install(FILES kdabcrm-mime.xml DESTINATION ${XDG_MIME_INSTALL_DIR})
update_xdg_mimetypes(${XDG_MIME_INSTALL_DIR})
