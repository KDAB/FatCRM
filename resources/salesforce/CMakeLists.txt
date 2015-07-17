#Salesforce Resource

set(salesforceresource_SRCS
  salesforcecontactshandler.cpp
  moduledebuginterface.cpp
  salesforcemodulehandler.cpp
  resourcedebuginterface.cpp
  salesforceconfigdialog.cpp
  salesforceresource.cpp
)

kde4_add_ui_files(salesforceresource_SRCS
  salesforceconfigdialog.ui
)

install(FILES salesforceresource.desktop DESTINATION "${CMAKE_INSTALL_PREFIX}/share/akonadi/agents")

kde4_add_kcfg_files(salesforceresource_SRCS settings.kcfgc)
kcfg_generate_dbus_interface(${CMAKE_CURRENT_SOURCE_DIR}/salesforceresource.kcfg
  org.kde.Akonadi.SalesForce.Settings
)
qt4_add_dbus_adaptor(salesforceresource_SRCS
  ${CMAKE_CURRENT_BINARY_DIR}/org.kde.Akonadi.SalesForce.Settings.xml settings.h Settings
)

kdsoap_generate_soap_bindings(salesforceresource_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/salesforce-partner.wsdl salesforcesoap)

kde4_add_executable(akonadi_salesforce_resource RUN_UNINSTALLED ${salesforceresource_SRCS})

target_link_libraries(akonadi_salesforce_resource
  ${KDSoap_LIBRARIES}
  ${KDE4_AKONADI_LIBS}
  ${KDEPIMLIBS_KABC_LIBS}
  ${KDE4_KDECORE_LIBS}
  ${QT_QTDBUS_LIBRARY}
  ${QT_QTCORE_LIBRARY}
)

install(TARGETS akonadi_salesforce_resource ${INSTALL_TARGETS_DEFAULT_ARGS})
