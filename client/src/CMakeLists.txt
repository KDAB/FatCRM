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
    models/filterproxymodel.cpp
    models/itemstreemodel.cpp
    models/referenceddatamodel.cpp
    details/detailswidget.cpp
    details/details.cpp
    details/accountdetails.cpp
    details/campaigndetails.cpp
    details/contactdetails.cpp
    details/leaddetails.cpp
    details/opportunitydetails.cpp
    dialogs/detailsdialog.cpp
    dialogs/resourceconfigdialog.cpp
    dialogs/shutdowndialog.cpp
    utilities/dbuswinidprovider.cpp
    utilities/editcalendarbutton.cpp
    utilities/referenceddata.cpp
)

kde4_add_ui_files(sugarclient_SRCS
    app/mainwindow.ui
    pages/page.ui
    details/accountdetails.ui
    details/detailswidget.ui
    details/opportunitydetails.ui
    dialogs/detailsdialog.ui
    dialogs/resourceconfigdialog.ui
    dialogs/shutdowndialog.ui
)

kde4_add_executable(sugarclient ${sugarclient_SRCS})

target_link_libraries(sugarclient
    kdcrmdata
    ${KDE4_KDECORE_LIBS}
    ${KDE4_AKONADI_LIBS}
    ${KDEPIMLIBS_KABC_LIBS}
    ${QT_QTGUI_LIBS}
)

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${KDE4_ENABLE_EXCEPTIONS}" )

install(TARGETS sugarclient ${INSTALL_TARGETS_DEFAULT_ARGS} )

########### install files ###############

install( FILES sugarclient.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR} )
