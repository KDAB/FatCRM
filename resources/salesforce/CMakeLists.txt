project(salesforce)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")

# search packages used by KDE
find_package (KDE4 REQUIRED)
include(KDE4Defaults)
include(MacroLibrary)
include(MacroOptionalAddSubdirectory)
include(CheckIncludeFiles)
find_package (KdepimLibs REQUIRED)

find_program(XSLTPROC_EXECUTABLE xsltproc)
macro_log_feature(XSLTPROC_EXECUTABLE "xsltproc" "The command line XSLT processor from libxslt" "http://xmlsoft.org/XSLT/" FALSE "" "Needed for building Akonadi resources. Recommended.")

# To ensure KDSoap is found, add its base directory to CMAKE_PREFIX_PATH
find_package(KDSoap REQUIRED)
#TODO macro_log_feature for KDSoap

if (XSLTPROC_EXECUTABLE)
  # generates a D-Bus interface description from a KConfigXT file
  macro( kcfg_generate_dbus_interface _kcfg _name )
    add_custom_command(
      OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${_name}.xml
      COMMAND ${XSLTPROC_EXECUTABLE} --stringparam interfaceName ${_name}
              ${KDEPIMLIBS_DATA_DIR}/akonadi-kde/kcfg2dbus.xsl
              ${_kcfg}
              > ${CMAKE_CURRENT_BINARY_DIR}/${_name}.xml
      DEPENDS ${KDEPIMLIBS_DATA_DIR}/akonadi-kde/kcfg2dbus.xsl
              ${_kcfg}
    )
  endmacro( kcfg_generate_dbus_interface )
endif (XSLTPROC_EXECUTABLE)

if (KDSoap_KDWSDL2CPP)
  # generate SOAP bindings for a wsdl file
  macro ( kdsoap_generate_soap_bindings _wsdl _name )
    add_custom_command(
      OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${_name}.h
      COMMAND ${KDSoap_KDWSDL2CPP} -o ${_name}.h
              ${_wsdl}
      DEPENDS ${_wsdl}
    )
    add_custom_command(
      OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${_name}.cpp
      COMMAND ${KDSoap_KDWSDL2CPP} -o ${_name}.cpp -impl ${_name}.h
              ${_wsdl}
      DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${_name}.h
              ${_wsdl}
    )
    QT4_GENERATE_MOC( ${CMAKE_CURRENT_BINARY_DIR}/${_name}.h ${_name}_moc.cpp )
  endmacro( kdsoap_generate_soap_bindings )
endif (KDSoap_KDWSDL2CPP)

if(WIN32)
    set(LIB_INSTALL_DIR ${LIB_INSTALL_DIR}
                        RUNTIME DESTINATION ${BIN_INSTALL_DIR}
                        LIBRARY DESTINATION ${LIB_INSTALL_DIR}
                        ARCHIVE DESTINATION ${LIB_INSTALL_DIR} )
endif(WIN32)

set(KDE4_ICON_DIR ${KDE4_INSTALL_DIR}/share/icons)

include_directories(
    ${KDE4_INCLUDES}
    ${KDEPIMLIBS_INCLUDE_DIRS}
    ${KDSoap_INCLUDE_DIR}
)

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${KDE4_ENABLE_EXCEPTIONS}" )

########### next target ###############

set( salesforceresource_SRCS
    contactshandler.cpp
    modulehandler.cpp
    resourcedebuginterface.cpp
    salesforceconfigdialog.cpp
    salesforceresource.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/salesforcesoap.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/salesforcesoap_moc.cpp
)

kde4_add_ui_files( salesforceresource_SRCS
    salesforceconfigdialog.ui
)

install( FILES salesforceresource.desktop DESTINATION "${CMAKE_INSTALL_PREFIX}/share/akonadi/agents" )

kde4_add_kcfg_files(salesforceresource_SRCS settings.kcfgc)
kcfg_generate_dbus_interface(${CMAKE_CURRENT_SOURCE_DIR}/salesforceresource.kcfg org.kde.Akonadi.SugarCRM.Settings)
qt4_add_dbus_adaptor(salesforceresource_SRCS
    ${CMAKE_CURRENT_BINARY_DIR}/org.kde.Akonadi.SugarCRM.Settings.xml settings.h Settings
)

kdsoap_generate_soap_bindings( ${CMAKE_CURRENT_SOURCE_DIR}/salesforce-partner.wsdl salesforcesoap )

kde4_add_executable(akonadi_salesforce_resource RUN_UNINSTALLED ${salesforceresource_SRCS})

target_link_libraries(akonadi_salesforce_resource
    ${KDSoap_LIBRARIES}
    ${KDE4_AKONADI_LIBS}
    ${KDEPIMLIBS_KABC_LIBS}
    ${KDE4_KDECORE_LIBS}
    ${QT_QTDBUS_LIBRARY}
    ${QT_QTCORE_LIBRARY})

install(TARGETS akonadi_salesforce_resource ${INSTALL_TARGETS_DEFAULT_ARGS})
