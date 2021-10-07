set(SHARED_MIME_INFO_MINIMUM_VERSION "0.20")
find_package(SharedMimeInfo REQUIRED)

add_library(kdcrmdata SHARED
    enumdefinitionattribute.cpp
    enumdefinitions.cpp
    kdcrmutils.cpp
    kdcrmfields.cpp
    sugaraccount.cpp
    sugaraccountio.cpp
    sugarcontactwrapper.cpp
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
generate_export_header(kdcrmdata BASE_NAME kdcrmdata)

target_link_libraries(kdcrmdata
PUBLIC
  Qt5::Core
  KF5::AkonadiCore      # Akonadi::Attribute
  KF5::I18n
  KF5::Contacts
)

set_target_properties(kdcrmdata PROPERTIES
  VERSION ${FatCRM_SOVERSION}
  SOVERSION ${FatCRM_SOVERSION}
)

target_include_directories(kdcrmdata PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>")

install(TARGETS kdcrmdata ${INSTALL_TARGETS_DEFAULT_ARGS})

### next target
add_library(akonadi_serializer_sugaraccount MODULE serializerpluginsugaraccount.cpp)
target_link_libraries(akonadi_serializer_sugaraccount PRIVATE
  kdcrmdata
  KF5::AkonadiCore
)
install(TARGETS akonadi_serializer_sugaraccount DESTINATION ${PLUGIN_INSTALL_DIR})

### next target
add_library(akonadi_serializer_sugaropportunity MODULE serializerpluginsugaropportunity.cpp)
target_link_libraries(akonadi_serializer_sugaropportunity PRIVATE
  kdcrmdata
  KF5::AkonadiCore
)
install(TARGETS akonadi_serializer_sugaropportunity DESTINATION ${PLUGIN_INSTALL_DIR})

### next target
add_library(akonadi_serializer_sugarlead MODULE serializerpluginsugarlead.cpp)
target_link_libraries(akonadi_serializer_sugarlead PRIVATE
  kdcrmdata
  KF5::AkonadiCore
)
install(TARGETS akonadi_serializer_sugarlead DESTINATION ${PLUGIN_INSTALL_DIR})

### next target
add_library(akonadi_serializer_sugarcampaign MODULE serializerpluginsugarcampaign.cpp)
target_link_libraries(akonadi_serializer_sugarcampaign
  kdcrmdata
  KF5::AkonadiCore
)
install(TARGETS akonadi_serializer_sugarcampaign DESTINATION ${PLUGIN_INSTALL_DIR})

### next target
add_library(akonadi_serializer_sugarnote MODULE serializerpluginsugarnote.cpp)
target_link_libraries(akonadi_serializer_sugarnote
  kdcrmdata
  KF5::AkonadiCore
)
install(TARGETS akonadi_serializer_sugarnote DESTINATION ${PLUGIN_INSTALL_DIR})

### next target
add_library(akonadi_serializer_sugaremail MODULE serializerpluginsugaremail.cpp)
target_link_libraries(akonadi_serializer_sugaremail
  kdcrmdata
  KF5::AkonadiCore
)
install(TARGETS akonadi_serializer_sugaremail DESTINATION ${PLUGIN_INSTALL_DIR})

### next target
add_library(akonadi_serializer_sugardocument MODULE serializerpluginsugardocument.cpp)
target_link_libraries(akonadi_serializer_sugardocument
  kdcrmdata
  KF5::AkonadiCore
)
install(TARGETS akonadi_serializer_sugardocument DESTINATION ${PLUGIN_INSTALL_DIR})

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
