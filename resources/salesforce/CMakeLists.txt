#Salesforce Resource

set(salesforceresource_SRCS
  salesforcecontactshandler.cpp
  moduledebuginterface.cpp
  salesforcemodulehandler.cpp
  resourcedebuginterface.cpp
  salesforceconfigdialog.cpp
  salesforceresource.cpp
)

ki18n_wrap_ui(salesforceresource_SRCS
  salesforceconfigdialog.ui
)

install(FILES salesforceresource.desktop DESTINATION "${CMAKE_INSTALL_PREFIX}/share/akonadi/agents")

kconfig_add_kcfg_files(salesforceresource_SRCS settings.kcfgc)
kcfg_generate_dbus_interface(${CMAKE_CURRENT_SOURCE_DIR}/salesforceresource.kcfg
  org.kde.Akonadi.SalesForce.Settings
)
qt5_add_dbus_adaptor(salesforceresource_SRCS
  ${CMAKE_CURRENT_BINARY_DIR}/org.kde.Akonadi.SalesForce.Settings.xml settings.h Settings
)

kdsoap_generate_soap_bindings(salesforceresource_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/salesforce-partner.wsdl salesforcesoap)

add_executable(akonadi_salesforce_resource RUN_UNINSTALLED ${salesforceresource_SRCS})

target_link_libraries(akonadi_salesforce_resource
  ${KDSoap_LIBRARIES}
  ${KDE4_AKONADI_LIBS}
  KF5::Contacts
  KF5::KDELibs4Support
  Qt5::DBus
  Qt5::Core
)

install(TARGETS akonadi_salesforce_resource ${INSTALL_TARGETS_DEFAULT_ARGS})
