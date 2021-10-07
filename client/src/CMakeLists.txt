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
  views
  widgets
)

ecm_qt_declare_logging_category(app_SRCS HEADER fatcrm_client_debug.h IDENTIFIER FATCRM_CLIENT_LOG CATEGORY_NAME log_fatcrm_client)

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

add_library(fatcrmprivate
  app/clientsettings.cpp
  app/mainwindow.cpp
  app/mainwindow.ui
  details/accountdetails.cpp
  details/accountdetails.ui
  details/campaigndetails.cpp
  details/campaigndetails.ui
  details/contactdetails.cpp
  details/contactdetails.ui
  details/details.cpp
  details/leaddetails.cpp
  details/leaddetails.ui
  details/opportunitydetails.cpp
  details/opportunitydetails.ui
  dialogs/aboutdialog.cpp
  dialogs/aboutdialog.ui
  dialogs/accountimportpage.cpp
  dialogs/accountimportpage.ui
  dialogs/addsearchdialog.cpp
  dialogs/addsearchdialog.ui
  dialogs/configurationdialog.cpp
  dialogs/configurationdialog.ui
  dialogs/contactsimportpage.cpp
  dialogs/contactsimportpage.ui
  dialogs/contactsimportwizard.cpp
  dialogs/documentswindow.cpp
  dialogs/documentswindow.ui
  dialogs/editlistdialog.cpp
  dialogs/fatcrminputdialog.cpp
  dialogs/itemeditwidgetbase.cpp
  dialogs/noteswindow.cpp
  dialogs/noteswindow.ui
  dialogs/resourceconfigdialog.cpp
  dialogs/resourceconfigdialog.ui
  dialogs/searchesdialog.cpp
  dialogs/searchesdialog.ui
  dialogs/selectitemdialog.cpp
  dialogs/selectitemdialog.ui
  dialogs/simpleitemeditwidget.cpp
  dialogs/simpleitemeditwidget.ui
  dialogs/tabbeditemeditwidget.cpp
  dialogs/tabbeditemeditwidget.ui
  fatcrm_client_debug.cpp
  models/filterproxymodel.cpp
  models/itemstreemodel.cpp
  models/opportunityfilterproxymodel.cpp
  models/referenceddatamodel.cpp
  pages/accountspage.cpp
  pages/campaignspage.cpp
  pages/contactfilterwidget.cpp
  pages/contactspage.cpp
  pages/leadspage.cpp
  pages/opportunitiespage.cpp
  pages/opportunityfilterwidget.cpp
  pages/opportunityfilterwidget.ui
  pages/page.cpp
  pages/page.ui
  pages/reportpage.cpp
  pages/reportpage.ui
  reports/createlinksproxymodel.cpp
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
  utilities/externalopen.cpp
  utilities/itemdataextractor.cpp
  utilities/keypresseventlistview.cpp
  utilities/kjobprogresstracker.cpp
  utilities/leaddataextractor.cpp
  utilities/linkeditemsrepository.cpp
  utilities/loadingoverlay.cpp
  utilities/modelrepository.cpp
  utilities/openedwidgetsrepository.cpp
  utilities/opportunitydataextractor.cpp
  utilities/opportunityfiltersettings.cpp
  utilities/qcsvreader.cpp
  utilities/referenceddata.cpp
  views/itemstreeview.cpp
  widgets/associateddatawidget.cpp
  widgets/associateddatawidget.ui
  widgets/betterplaintextedit.cpp
  widgets/nullabledatecombobox.cpp

  resources.qrc
  ../data/flags/flags.qrc

  ${app_SRCS}
)
generate_export_header(fatcrmprivate BASE_NAME fatcrmprivate)
target_link_libraries(fatcrmprivate
    PRIVATE
        KDReports::kdreports
        KF5::AkonadiWidgets
        KF5::DBusAddons
        KF5::GuiAddons
        KF5::KIOWidgets
        KF5::TextWidgets
        Qt5::PrintSupport
        KF5::IconThemes
    PUBLIC
        kdcrmdata
        KF5::WidgetsAddons # KDateComboBox
        KF5::Contacts
)

if (PhoneNumber_FOUND AND ICU_FOUND)
    target_link_libraries(fatcrmprivate
      PUBLIC
        ${PhoneNumber_LIBRARIES}
        ${PhoneNumberGeoCoding_LIBRARIES}
        ICU::uc
     )
endif()

ecm_add_app_icon(fatcrm_app_icon_SRCS ICONS ${fatcrm_ICONS})
add_executable(fatcrm ${fatcrm_app_icon_SRCS} app/main.cpp)
target_link_libraries(fatcrm PRIVATE
    fatcrmprivate
    KDReports::kdreports
    KF5::DBusAddons
    KF5::WidgetsAddons
)

install(TARGETS fatcrmprivate ${INSTALL_TARGETS_DEFAULT_ARGS})
install(TARGETS fatcrm ${INSTALL_TARGETS_DEFAULT_ARGS})

########### install files ###############

install(FILES org.kde.fatcrm.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
