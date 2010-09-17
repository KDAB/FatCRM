include_directories ( ${CMAKE_SOURCE_DIR} )

set(sugarclient_SRCS
    main.cpp
    sugarclient.cpp
    page.cpp
    accountspage.cpp
    campaignspage.cpp
    contactspage.cpp
    leadspage.cpp
    opportunitiespage.cpp
    accountstreemodel.cpp
    accountsfilterproxymodel.cpp
    campaignstreemodel.cpp
    campaignsfilterproxymodel.cpp
    leadstreemodel.cpp
    leadsfilterproxymodel.cpp
    opportunitiestreemodel.cpp
    opportunitiesfilterproxymodel.cpp
    detailswidget.cpp
    details.cpp
    accountdetails.cpp
    campaigndetails.cpp
    contactdetails.cpp
    leaddetails.cpp
    opportunitydetails.cpp
    editcalendarbutton.cpp
    resourceconfigdialog.cpp
    shutdowndialog.cpp
    dbuswinidprovider.cpp
)

kde4_add_ui_files(sugarclient_SRCS
    mainwindow.ui
    page.ui  
    detailswidget.ui 
    resourceconfigdialog.ui
    shutdowndialog.ui
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
