include_directories ( ${CMAKE_SOURCE_DIR} app details dialogs models pages utilities )

set(sugarclient_SRCS
    app/sugarclient.cpp
    pages/page.cpp
    pages/accountspage.cpp
    pages/campaignspage.cpp
    pages/contactspage.cpp
    pages/leadspage.cpp
    pages/opportunitiespage.cpp
    pages/opportunityfilterwidget.cpp
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
    dialogs/detailsdialog.cpp
    dialogs/resourceconfigdialog.cpp
    utilities/dbuswinidprovider.cpp
    utilities/editcalendarbutton.cpp
    utilities/referenceddata.cpp
)

kde4_add_ui_files(sugarclient_SRCS
    app/mainwindow.ui
    pages/page.ui
    pages/opportunityfilterwidget.ui
    details/accountdetails.ui
    details/campaigndetails.ui
    details/contactdetails.ui
    details/detailswidget.ui
    details/leaddetails.ui
    details/opportunitydetails.ui
    dialogs/detailsdialog.ui
    dialogs/resourceconfigdialog.ui
)

kde4_add_library(sugarclientprivate ${sugarclient_SRCS})

target_link_libraries(sugarclientprivate
    kdcrmdata
    ${KDE4_KDECORE_LIBS}
    ${KDE4_AKONADI_LIBS}
    ${KDEPIMLIBS_KABC_LIBS}
    ${QT_QTGUI_LIBS}
)

kde4_add_executable(sugarclient app/main.cpp)

target_link_libraries(sugarclient
	sugarclientprivate
)

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${KDE4_ENABLE_EXCEPTIONS}" )

install(TARGETS sugarclientprivate ${INSTALL_TARGETS_DEFAULT_ARGS} )
install(TARGETS sugarclient ${INSTALL_TARGETS_DEFAULT_ARGS} )

########### install files ###############

install( FILES sugarclient.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR} )
