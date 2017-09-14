include_directories(
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_BINARY_DIR}
  app
  details
  dialogs
  models
  pages
  reports
  utilities
  widgets
  views
)

set(app_SRCS
  fatcrm_client_debug.cpp
  app/mainwindow.cpp
  app/clientsettings.cpp
  pages/page.cpp
  pages/accountspage.cpp
  pages/campaignspage.cpp
  pages/contactspage.cpp
  pages/leadspage.cpp
  pages/opportunitiespage.cpp
  pages/opportunityfilterwidget.cpp
  pages/reportpage.cpp
  models/filterproxymodel.cpp
  models/itemstreemodel.cpp
  models/opportunityfilterproxymodel.cpp
  models/referenceddatamodel.cpp
  details/details.cpp
  details/accountdetails.cpp
  details/campaigndetails.cpp
  details/contactdetails.cpp
  details/leaddetails.cpp
  details/opportunitydetails.cpp
  dialogs/accountimportpage.cpp
  dialogs/contactsimportpage.cpp
  dialogs/contactsimportwizard.cpp
  dialogs/configurationdialog.cpp
  dialogs/itemeditwidgetbase.cpp
  dialogs/simpleitemeditwidget.cpp
  dialogs/documentswindow.cpp
  dialogs/editlistdialog.cpp
  dialogs/fatcrminputdialog.cpp
  dialogs/noteswindow.cpp
  dialogs/resourceconfigdialog.cpp
  dialogs/selectitemdialog.cpp
  dialogs/tabbeditemeditwidget.cpp
  reports/rearrangecolumnsproxymodel.cpp
  reports/reportgenerator.cpp
  utilities/accountdataextractor.cpp
  utilities/accountrepository.cpp
  utilities/campaigndataextractor.cpp
  utilities/collectionmanager.cpp
  utilities/contactdataextractor.cpp
  utilities/contactsimporter.cpp
  utilities/dbusinvokerinterface.cpp
  utilities/dbuswinidprovider.cpp
  utilities/editcalendarbutton.cpp
  utilities/enums.cpp
  utilities/loadingoverlay.cpp
  utilities/itemdataextractor.cpp
  utilities/keypresseventlistview.cpp
  utilities/kjobprogresstracker.cpp
  utilities/leaddataextractor.cpp
  utilities/linkeditemsrepository.cpp
  utilities/modelrepository.cpp
  utilities/openedwidgetsrepository.cpp
  utilities/opportunitydataextractor.cpp
  utilities/opportunityfiltersettings.cpp
  utilities/qcsvreader.cpp
  utilities/referenceddata.cpp
  views/itemstreeview.cpp
  widgets/associateddatawidget.cpp
  widgets/betterplaintextedit.cpp
  widgets/qdateeditex.cpp
)
ecm_qt_declare_logging_category(app_SRCS HEADER fatcrm_client_debug.h IDENTIFIER FATCRM_CLIENT_LOG CATEGORY_NAME log_fatcrm_client)

qt5_wrap_ui(app_SRCS
  app/mainwindow.ui
  pages/page.ui
  pages/reportpage.ui
  pages/opportunityfilterwidget.ui
  details/accountdetails.ui
  details/campaigndetails.ui
  details/contactdetails.ui
  details/leaddetails.ui
  details/opportunitydetails.ui
  dialogs/accountimportpage.ui
  dialogs/configurationdialog.ui
  dialogs/contactsimportpage.ui
  dialogs/documentswindow.ui
  dialogs/noteswindow.ui
  dialogs/resourceconfigdialog.ui
  dialogs/selectitemdialog.ui
  dialogs/simpleitemeditwidget.ui
  dialogs/tabbeditemeditwidget.ui
  widgets/associateddatawidget.ui
)

#qt5_add_resources(app_SRCS "${CMAKE_SOURCE_DIR}/images/hi*-apps-fatcrm.png")

qt5_add_resources(app_SRCS
  icons/icons.qrc
  widgets/widgets.qrc
)

# Generate DBus interface in order to find out the config of the sugar resource (for the web link)
# TODO generalize to salesforce somehow one day (maybe a folder attribute instead?)
kcfg_generate_dbus_interface(${CMAKE_CURRENT_SOURCE_DIR}/../../resources/sugarcrm/sugarcrmresource.kcfg org.kde.Akonadi.SugarCRM.Settings)
qt5_add_dbus_interface(app_SRCS
  ${CMAKE_CURRENT_BINARY_DIR}/org.kde.Akonadi.SugarCRM.Settings.xml sugarresourcesettings SugarSettings
)

qt5_generate_dbus_interface(${CMAKE_CURRENT_SOURCE_DIR}/../../resources/sugarcrm/itemtransferinterface.h org.kde.Akonadi.SugarCRM.ItemTransfer.xml)
qt5_add_dbus_interface(app_SRCS
  ${CMAKE_CURRENT_BINARY_DIR}/org.kde.Akonadi.SugarCRM.ItemTransfer.xml sugarresourceitemtransfer SugarItemTransfer
)

add_library(fatcrmprivate ${app_SRCS})
generate_export_header(fatcrmprivate BASE_NAME fatcrmprivate)
target_link_libraries(fatcrmprivate
  kdcrmdata
  KF5::AkonadiWidgets
  KF5::Contacts
  KF5::KDELibs4Support
  KF5::DBusAddons
  )


target_link_libraries(fatcrmprivate
  ${KDReports_LIBRARIES}
)

add_executable(fatcrm app/main.cpp)

target_link_libraries(fatcrm
  fatcrmprivate
)

install(TARGETS fatcrmprivate ${INSTALL_TARGETS_DEFAULT_ARGS})
install(TARGETS fatcrm ${INSTALL_TARGETS_DEFAULT_ARGS})

########### install files ###############

install(FILES org.kde.fatcrm.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
