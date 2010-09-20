include_directories ( ${CMAKE_SOURCE_DIR} app details dialogs models pages utilities )

set(sugarclient_SRCS
    app/main.cpp
    app/sugarclient.cpp
    pages/page.cpp
    pages/accountspage.cpp
    pages/campaignspage.cpp
    pages/contactspage.cpp
    pages/leadspage.cpp
    pages/opportunitiespage.cpp
    models/accountstreemodel.cpp
    models/accountsfilterproxymodel.cpp
    models/campaignstreemodel.cpp
    models/campaignsfilterproxymodel.cpp
    models/contactstreemodel.cpp
    models/contactsfilterproxymodel.cpp
    models/leadstreemodel.cpp
    models/leadsfilterproxymodel.cpp
    models/opportunitiestreemodel.cpp
    models/opportunitiesfilterproxymodel.cpp
    details/detailswidget.cpp
    details/details.cpp
    details/accountdetails.cpp
    details/campaigndetails.cpp
    details/contactdetails.cpp
    details/leaddetails.cpp
    details/opportunitydetails.cpp
    dialogs/resourceconfigdialog.cpp
    dialogs/shutdowndialog.cpp
    utilities/editcalendarbutton.cpp
    utilities/dbuswinidprovider.cpp
)

kde4_add_ui_files(sugarclient_SRCS
    app/mainwindow.ui
    pages/page.ui  
    details/detailswidget.ui 
    dialogs/resourceconfigdialog.ui
    dialogs/shutdowndialog.ui
)

kde4_add_executable(sugarclient ${sugarclient_SRCS})

target_link_libraries(sugarclient
    kdcrmdata
    ${KDE4_KDECORE_LIBS}
    ${KDE4_AKONADI_LIBS}
    ${KDEPIMLIBS_KABC_LIBS}
    ${KDEPIMLIBS_AKONADI_CONTACT_LIBS}
    ${QT_QTGUI_LIBS}
)

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${KDE4_ENABLE_EXCEPTIONS}" )

install(TARGETS sugarclient ${INSTALL_TARGETS_DEFAULT_ARGS} )

########### install files ###############

install( FILES sugarclient.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR} )
