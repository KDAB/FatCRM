#SugarCRM Resource

add_definitions(-DTRANSLATION_DOMAIN=\"akonadi_sugarcrm_resource\")

set(sugarcrmresource_SRCS)

ecm_qt_declare_logging_category(sugarcrmresource_SRCS HEADER sugarcrmresource_debug.h IDENTIFIER FATCRM_SUGARCRMRESOURCE_LOG CATEGORY_NAME log_fatcrm_sugarcrmresource)

install(FILES sugarcrmresource.desktop DESTINATION "${KDE_INSTALL_DATAROOTDIR}/akonadi/agents")

kconfig_add_kcfg_files(sugarcrmresource_SRCS settings.kcfgc)
kcfg_generate_dbus_interface(${CMAKE_CURRENT_SOURCE_DIR}/sugarcrmresource.kcfg org.kde.Akonadi.SugarCRM.Settings)
qt5_add_dbus_adaptor(sugarcrmresource_SRCS
  ${CMAKE_CURRENT_BINARY_DIR}/org.kde.Akonadi.SugarCRM.Settings.xml settings.h Settings
)

kdsoap_generate_wsdl(sugarcrmresource_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/sugar41.wsdl)

add_executable(akonadi_sugarcrm_resource main_sugarcrmresource.cpp)
set_target_properties(akonadi_sugarcrm_resource PROPERTIES MACOSX_BUNDLE FALSE)

add_library(akonadi_sugarcrm_resource_private STATIC
    ${sugarcrmresource_SRCS}

    accountshandler.cpp
    campaignshandler.cpp
    contactshandler.cpp
    createentryjob.cpp
    currency.cpp
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
    modulename.cpp
    noteshandler.cpp
    opportunitieshandler.cpp
    passwordhandler.cpp
    resourcedebuginterface.cpp
    sugarconfigdialog.cpp
    sugarconfigdialog.ui
    sugarcrmresource.cpp
    sugarjob.cpp
    sugarprotocolbase.cpp
    sugarsession.cpp
    sugarsoapprotocol.cpp
    taskaccessorpair.cpp
    taskshandler.cpp
    tests/sugarmockprotocol.cpp
    updateentryjob.cpp

    # work with local implementations until available from kdepimlibs
    conflicthandler.cpp
    conflictresolvedialog.cpp
)

target_link_libraries(akonadi_sugarcrm_resource_private
    PRIVATE
        kdcrmdata
        KDSoap::kdsoap
        KF5::CalendarCore
        Qt5::DBus
        Qt5::Core
        Qt5::Network
        KF5::CalendarCore
        KF5::AkonadiContact
        KF5::WindowSystem
    PUBLIC
        KF5::AkonadiAgentBase
        KF5::Contacts
)
if(USE_KWALLET)
  target_link_libraries(akonadi_sugarcrm_resource_private PRIVATE
    KF5::Wallet
  )
endif()
target_link_libraries(akonadi_sugarcrm_resource_private PRIVATE KDReports::kdreports)

target_link_libraries(akonadi_sugarcrm_resource PRIVATE
  akonadi_sugarcrm_resource_private
)


install(TARGETS akonadi_sugarcrm_resource ${INSTALL_TARGETS_DEFAULT_ARGS})

add_subdirectory(tests)
