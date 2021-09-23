#Salesforce Resource

add_definitions(-DTRANSLATION_DOMAIN=\"akonadi_salesforce_resource\")

set(salesforceresource_SRCS)
ecm_qt_declare_logging_category(salesforceresource_SRCS HEADER salesforceresource_debug.h IDENTIFIER FATCRM_SALESFORCERESOURCE_LOG CATEGORY_NAME log_fatcrm_salesforceresource)

install(FILES salesforceresource.desktop DESTINATION "${CMAKE_INSTALL_PREFIX}/share/akonadi/agents")

kconfig_add_kcfg_files(salesforceresource_SRCS settings.kcfgc)
kcfg_generate_dbus_interface(${CMAKE_CURRENT_SOURCE_DIR}/salesforceresource.kcfg
  org.kde.Akonadi.SalesForce.Settings
)
qt5_add_dbus_adaptor(salesforceresource_SRCS
  ${CMAKE_CURRENT_BINARY_DIR}/org.kde.Akonadi.SalesForce.Settings.xml settings.h Settings
)

kdsoap_generate_wsdl(salesforceresource_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/salesforce-partner.wsdl)

add_executable(akonadi_salesforce_resource
    ${salesforceresource_SRCS}
    moduledebuginterface.cpp
    resourcedebuginterface.cpp
    salesforceconfigdialog.cpp
    salesforceconfigdialog.ui
    salesforcecontactshandler.cpp
    salesforcemodulehandler.cpp
    salesforceresource.cpp
)
set_target_properties(akonadi_salesforce_resource PROPERTIES MACOSX_BUNDLE FALSE)

target_link_libraries(akonadi_salesforce_resource PRIVATE
  kdcrmdata
  KDSoap::kdsoap
  KF5::Contacts
  Qt5::DBus
  Qt5::Core
  KF5::AkonadiCore
  KF5::AkonadiAgentBase
  KF5::I18n
  KF5::KIOWidgets
  KF5::WindowSystem
)

install(TARGETS akonadi_salesforce_resource ${INSTALL_TARGETS_DEFAULT_ARGS})
