# To ensure KDSoap is found, add its base directory to CMAKE_PREFIX_PATH
set(MIN_KDSOAP_VER "1.6.0")
find_package(KDSoap ${MIN_KDSOAP_VER} CONFIG)
set_package_properties(KDSoap PROPERTIES
    DESCRIPTION "KDSoap, Qt SOAP library by KDAB.com"
    TYPE REQUIRED URL "https://www.kdab.com/kdsoap"
    PURPOSE "Required to access the SugarCRM SOAP webservice."
)
set(KSWSDL2CPP_OPTION -namespace KDSoapGenerated)

if(WIN32)
  set(
    LIB_INSTALL_DIR ${LIB_INSTALL_DIR}
    RUNTIME DESTINATION ${BIN_INSTALL_DIR}
    LIBRARY DESTINATION ${LIB_INSTALL_DIR}
    ARCHIVE DESTINATION ${LIB_INSTALL_DIR}
  )
endif()

include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(salesforce)
add_subdirectory(sugarcrm)
