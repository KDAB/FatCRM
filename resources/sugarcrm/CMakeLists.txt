#SugarCRM Resource

add_definitions(-DTRANSLATION_DOMAIN=\"akonadi_sugarcrm_resource\")

set(sugarcrmresource_SRCS
  accountshandler.cpp
  campaignshandler.cpp
  contactshandler.cpp
  createentryjob.cpp
  deleteentryjob.cpp
  documentshandler.cpp
  emailshandler.cpp
  fetchentryjob.cpp
  itemtransferinterface.cpp
  leadshandler.cpp
  listentriesjob.cpp
  listentriesscope.cpp
  listmodulesjob.cpp
  loginjob.cpp
  moduledebuginterface.cpp
  modulehandler.cpp
  noteshandler.cpp
  opportunitieshandler.cpp
  passwordhandler.cpp
  resourcedebuginterface.cpp
  sugarconfigdialog.cpp
  sugarcrmresource.cpp
  sugarjob.cpp
  sugarsession.cpp
  updateentryjob.cpp
  taskshandler.cpp
  taskaccessorpair.cpp
  sugarprotocolbase.cpp
  sugarsoapprotocol.cpp
  tests/sugarmockprotocol.cpp
  modulename.cpp

  # work with local implementations until available from kdepimlibs
  conflicthandler.cpp
  conflictresolvedialog.cpp
)

ecm_qt_declare_logging_category(sugarcrmresource_SRCS HEADER sugarcrmresource_debug.h IDENTIFIER FATCRM_SUGARCRMRESOURCE_LOG CATEGORY_NAME log_fatcrm_sugarcrmresource)

qt5_wrap_ui(sugarcrmresource_SRCS
  sugarconfigdialog.ui
)

install(FILES sugarcrmresource.desktop DESTINATION "${CMAKE_INSTALL_PREFIX}/share/akonadi/agents")

kconfig_add_kcfg_files(sugarcrmresource_SRCS settings.kcfgc)
kcfg_generate_dbus_interface(${CMAKE_CURRENT_SOURCE_DIR}/sugarcrmresource.kcfg org.kde.Akonadi.SugarCRM.Settings)
qt5_add_dbus_adaptor(sugarcrmresource_SRCS
  ${CMAKE_CURRENT_BINARY_DIR}/org.kde.Akonadi.SugarCRM.Settings.xml settings.h Settings
)

kdsoap_generate_soap_bindings(sugarcrmresource_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/sugar41.wsdl sugarsoap "")

add_executable(akonadi_sugarcrm_resource main_sugarcrmresource.cpp)

add_library(akonadi_sugarcrm_resource_private STATIC ${sugarcrmresource_SRCS})

target_link_libraries(akonadi_sugarcrm_resource_private
  kdcrmdata
  ${KDSoap_LIBRARIES}
  KF5::Contacts
  KF5::CalendarCore
  Qt5::DBus
  Qt5::Core
  Qt5::Network
  KF5::CalendarCore
  KF5::AkonadiAgentBase
  KF5::AkonadiContact
  KF5::WindowSystem
)
if(KCALENDARCORE_REQUIRES_KDE4SUPPORT)
  target_link_libraries(akonadi_sugarcrm_resource_private
    KF5::KDELibs4Support
  )
endif()
if(USE_KWALLET)
  target_link_libraries(akonadi_sugarcrm_resource_private
    KF5::Wallet
  )
endif()
target_link_libraries(akonadi_sugarcrm_resource_private
  ${KDReports_LIBRARIES}
)

target_link_libraries(akonadi_sugarcrm_resource
  akonadi_sugarcrm_resource_private
)


install(TARGETS akonadi_sugarcrm_resource ${INSTALL_TARGETS_DEFAULT_ARGS})

add_subdirectory(tests)
