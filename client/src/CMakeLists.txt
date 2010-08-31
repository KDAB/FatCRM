include_directories ( ${CMAKE_SOURCE_DIR} )

set(sugarclient_SRCS
    page.cpp
    contactspage.cpp
    accountspage.cpp
    accountstreemodel.cpp
    accountsfilterproxymodel.cpp
    opportunitiespage.cpp
    opportunitiestreemodel.cpp
    opportunitiesfilterproxymodel.cpp
    leadspage.cpp
    leadstreemodel.cpp
    leadsfilterproxymodel.cpp
    campaignspage.cpp
    campaignstreemodel.cpp
    campaignsfilterproxymodel.cpp
    abstractdetails.cpp
    contactdetails.cpp
    accountdetails.cpp
    opportunitydetails.cpp
    leaddetails.cpp
    campaigndetails.cpp
    editcalendarbutton.cpp
    main.cpp
    sugarclient.cpp
    resourceconfigdialog.cpp
    shutdowndialog.cpp
)

kde4_add_ui_files(sugarclient_SRCS
    page.ui
    contactspage.ui
    #accountspage.ui
    opportunitiespage.ui
    leadspage.ui
    campaignspage.ui
    contactdetails.ui
    accountdetails.ui
    opportunitydetails.ui
    leaddetails.ui
    campaigndetails.ui
    mainwindow.ui
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
