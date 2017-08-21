
# To ensure KDSoap is found, add its base directory to CMAKE_PREFIX_PATH
set(MIN_KDSOAP_VER "1.6.0")
find_package(KDSoap ${MIN_KDSOAP_VER} CONFIG QUIET) # first try KDSoapConfig.cmake
find_package(KDSoap ${MIN_KDSOAP_VER} REQUIRED) # then try FindKDSoap.cmake
set_package_properties(KDSoap PROPERTIES DESCRIPTION "KDSoap, Qt SOAP library by KDAB.com" TYPE REQUIRED URL "http://www.kdab.com/kdsoap" PURPOSE "Required to access the SugarCRM SOAP webservice.")


# TODO: Just use KDSOAP_GENERATE_WSDL?
if(KDSoap_CODEGENERATOR)
  # generate SOAP bindings for a wsdl file
  macro(kdsoap_generate_soap_bindings _sources _wsdl _name _namespaceSuffix)
    add_custom_command(
      OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${_name}.h
      COMMAND ${KDSoap_CODEGENERATOR} -namespace KDSoapGenerated${_namespaceSuffix} -o ${_name}.h
              ${_wsdl}
      DEPENDS ${_wsdl}
    )
    add_custom_target(generate_${_name}_h DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${_name}.h)
    add_custom_command(
      OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${_name}.cpp
      COMMAND ${KDSoap_CODEGENERATOR} -o ${_name}.cpp -namespace KDSoapGenerated${_namespaceSuffix} -impl ${_name}.h
              ${_wsdl}
      DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${_name}.h
              ${_wsdl}
    )

    qt5_generate_moc(${CMAKE_CURRENT_BINARY_DIR}/${_name}.h ${CMAKE_CURRENT_BINARY_DIR}/${_name}_moc.cpp)
    set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/${_name}.cpp PROPERTIES SKIP_AUTOMOC TRUE)

    set(${_sources} ${${_sources}} ${CMAKE_CURRENT_BINARY_DIR}/${_name}.cpp ${CMAKE_CURRENT_BINARY_DIR}/${_name}.h ${CMAKE_CURRENT_BINARY_DIR}/${_name}_moc.cpp)
  endmacro()
endif()

if(WIN32)
  set(
    LIB_INSTALL_DIR ${LIB_INSTALL_DIR}
    RUNTIME DESTINATION ${BIN_INSTALL_DIR}
    LIBRARY DESTINATION ${LIB_INSTALL_DIR}
    ARCHIVE DESTINATION ${LIB_INSTALL_DIR}
  )
endif()

include_directories(${CMAKE_SOURCE_DIR} ${KDSoap_INCLUDE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

if(APPLE OR WIN32)
  set(_default_kwallet_option 0)
else()
  set(_default_kwallet_option 1)
endif()
option(USE_KWALLET "Use KWallet for password storage (default 1 on Unix, 0 on Windows/Mac)" ${_default_kwallet_option})
if(USE_KWALLET)
  find_package(KF5Wallet ${KF5_VERSION} CONFIG REQUIRED)
endif()
configure_file(config-kwallet.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-kwallet.h)

add_subdirectory(salesforce)
add_subdirectory(sugarcrm)
