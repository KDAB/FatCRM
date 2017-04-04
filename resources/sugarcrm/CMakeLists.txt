#SugarCRM Resource

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
  listdeletedentriesjob.cpp
  listentriesjob.cpp
  listentriesscope.cpp
  listmodulesjob.cpp
  loginjob.cpp
  moduledebuginterface.cpp
  modulehandler.cpp
  noteshandler.cpp
  opportunitieshandler.cpp
  passwordhandler.cpp
  referenceupdatejob.cpp
  resourcedebuginterface.cpp
  sugarconfigdialog.cpp
  sugarcrmresource.cpp
  sugarjob.cpp
  sugarsession.cpp
  updateentryjob.cpp
  taskshandler.cpp
  taskaccessorpair.cpp
  sugarprotocolbase.cpp

  # work with local implementations until available from kdepimlibs
  conflicthandler.cpp
  conflictresolvedialog.cpp
)

kde4_add_ui_files(sugarcrmresource_SRCS
  sugarconfigdialog.ui
)

install(FILES sugarcrmresource.desktop DESTINATION "${CMAKE_INSTALL_PREFIX}/share/akonadi/agents")

kde4_add_kcfg_files(sugarcrmresource_SRCS settings.kcfgc)
kcfg_generate_dbus_interface(${CMAKE_CURRENT_SOURCE_DIR}/sugarcrmresource.kcfg org.kde.Akonadi.SugarCRM.Settings)
qt4_add_dbus_adaptor(sugarcrmresource_SRCS
  ${CMAKE_CURRENT_BINARY_DIR}/org.kde.Akonadi.SugarCRM.Settings.xml settings.h Settings
)

kdsoap_generate_soap_bindings(sugarcrmresource_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/sugar.wsdl sugarsoap "")

kdsoap_generate_soap_bindings(sugarcrmresource_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/sugar41.wsdl sugarsoap41 "41")

kde4_add_executable(akonadi_sugarcrm_resource RUN_UNINSTALLED main_sugarcrmresource.cpp)

kde4_add_library(akonadi_sugarcrm_resource_private ${sugarcrmresource_SRCS})

target_link_libraries(akonadi_sugarcrm_resource_private
  kdcrmdata
  ${KDSoap_LIBRARIES}
  ${KDE4_AKONADI_KABC_LIBS}
  ${KDE4_AKONADI_LIBS}
  ${KDEPIMLIBS_KABC_LIBS}
  ${KDEPIMLIBS_KCALCORE_LIBS}
  ${KDE4_KDECORE_LIBS}
  ${QT_QTDBUS_LIBRARY}
  ${QT_QTCORE_LIBRARY}
)

target_link_libraries(akonadi_sugarcrm_resource_private
  ${KDReports_LIBRARIES}
)

target_link_libraries(akonadi_sugarcrm_resource
  akonadi_sugarcrm_resource_private
)


install(TARGETS akonadi_sugarcrm_resource ${INSTALL_TARGETS_DEFAULT_ARGS})
