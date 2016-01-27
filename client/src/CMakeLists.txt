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
  details/detailswidget.cpp
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
  dialogs/detailsdialog.cpp
  dialogs/editlistdialog.cpp
  dialogs/notesdialog.cpp
  dialogs/resourceconfigdialog.cpp
  reports/rearrangecolumnsproxymodel.cpp
  reports/reportgenerator.cpp
  utilities/accountrepository.cpp
  utilities/collectionmanager.cpp
  utilities/contactsimporter.cpp
  utilities/dbusinvokerinterface.cpp
  utilities/dbuswinidprovider.cpp
  utilities/editcalendarbutton.cpp
  utilities/enums.cpp
  utilities/notesrepository.cpp
  utilities/opportunityfiltersettings.cpp
  utilities/qcsvreader.cpp
  utilities/referenceddata.cpp
  views/itemstreeview.cpp
  widgets/betterplaintextedit.cpp
  widgets/qdateeditex.cpp
)

kde4_add_ui_files(app_SRCS
  app/mainwindow.ui
  pages/page.ui
  pages/reportpage.ui
  pages/opportunityfilterwidget.ui
  details/accountdetails.ui
  details/campaigndetails.ui
  details/contactdetails.ui
  details/detailswidget.ui
  details/leaddetails.ui
  details/opportunitydetails.ui
  dialogs/accountimportpage.ui
  dialogs/configurationdialog.ui
  dialogs/contactsimportpage.ui
  dialogs/detailsdialog.ui
  dialogs/notesdialog.ui
  dialogs/resourceconfigdialog.ui
)

kde4_add_app_icon(app_SRCS "${CMAKE_SOURCE_DIR}/images/hi*-apps-fatcrm.png")

qt4_add_resources(app_SRCS
  widgets/widgets.qrc
)
qt4_add_resources(app_SRCS
  icons/icons.qrc
)

# Generate DBus interface in order to find out the config of the sugar resource (for the web link)
# TODO generalize to salesforce somehow one day (maybe a folder attribute instead?)
kcfg_generate_dbus_interface(${CMAKE_CURRENT_SOURCE_DIR}/../../resources/sugarcrm/sugarcrmresource.kcfg org.kde.Akonadi.SugarCRM.Settings)
qt4_add_dbus_interface(app_SRCS
  ${CMAKE_CURRENT_BINARY_DIR}/org.kde.Akonadi.SugarCRM.Settings.xml sugarresourcesettings SugarSettings
)

kde4_add_library(fatcrmprivate ${app_SRCS})

target_link_libraries(fatcrmprivate
  kdcrmdata
  ${KDE4_KDECORE_LIBS}
  ${KDE4_AKONADI_LIBS}
  ${KDEPIMLIBS_KABC_LIBS}
  ${KDEPIMLIBS_KPIMUTILS_LIBS}
  ${QT_QTGUI_LIBS}
)

target_link_libraries(fatcrmprivate
  ${KDReports_LIBRARIES}
)

kde4_add_executable(fatcrm app/main.cpp)

target_link_libraries(fatcrm
  fatcrmprivate
)

install(TARGETS fatcrmprivate ${INSTALL_TARGETS_DEFAULT_ARGS})
install(TARGETS fatcrm ${INSTALL_TARGETS_DEFAULT_ARGS})

########### install files ###############

install(FILES fatcrm.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
