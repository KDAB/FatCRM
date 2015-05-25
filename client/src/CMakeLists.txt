include_directories ( ${CMAKE_SOURCE_DIR} app details dialogs models pages utilities widgets views )

set(sugarclient_SRCS
    app/sugarclient.cpp
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
    dialogs/configurationdialog.cpp
    dialogs/detailsdialog.cpp
    dialogs/editlistdialog.cpp
    dialogs/notesdialog.cpp
    dialogs/resourceconfigdialog.cpp
    utilities/collectionmanager.cpp
    utilities/contactsimporter.cpp
    utilities/dbuswinidprovider.cpp
    utilities/editcalendarbutton.cpp
    utilities/notesrepository.cpp
    utilities/qcsvreader.cpp
    utilities/referenceddata.cpp
    utilities/reportgenerator.cpp
    views/itemstreeview.cpp
    widgets/qdateeditex.cpp
)

kde4_add_ui_files(sugarclient_SRCS
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
    dialogs/configurationdialog.ui
    dialogs/detailsdialog.ui
    dialogs/notesdialog.ui
    dialogs/resourceconfigdialog.ui
)

qt4_add_resources(sugarclient_SRCS
    widgets/widgets.qrc
)
qt4_add_resources(sugarclient_SRCS
    icons/icons.qrc
)

# To find out the config of the sugar resource (for the web link) - TODO generalize to salesforce somehow
kcfg_generate_dbus_interface(${CMAKE_CURRENT_SOURCE_DIR}/../../resources/sugarcrm/sugarcrmresource.kcfg org.kde.Akonadi.SugarCRM.Settings)
qt4_add_dbus_interface(sugarclient_SRCS
	${CMAKE_CURRENT_BINARY_DIR}/org.kde.Akonadi.SugarCRM.Settings.xml sugarresourcesettings SugarSettings
  )

kde4_add_library(sugarclientprivate ${sugarclient_SRCS})

target_link_libraries(sugarclientprivate
    kdcrmdata
    ${KDE4_KDECORE_LIBS}
    ${KDE4_AKONADI_LIBS}
    ${KDEPIMLIBS_KABC_LIBS}
    ${QT_QTGUI_LIBS}
)

if (KDREPORTS_FOUND)
   target_link_libraries(sugarclientprivate ${KDReports_LIBRARIES})
endif()

kde4_add_executable(sugarclient app/main.cpp)

target_link_libraries(sugarclient
    sugarclientprivate
)

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${KDE4_ENABLE_EXCEPTIONS}" )

install(TARGETS sugarclientprivate ${INSTALL_TARGETS_DEFAULT_ARGS} )
install(TARGETS sugarclient ${INSTALL_TARGETS_DEFAULT_ARGS} )

########### install files ###############

install( FILES fatcrm.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR} )
