
# To ensure KDSoap is found, add its base directory to CMAKE_PREFIX_PATH
find_package(KDSoap CONFIG QUIET) # first try KDSoapConfig.cmake
find_package(KDSoap REQUIRED) # then try FindKDSoap.cmake
macro_log_feature(KDSoap "KD SOAP" "Qt SOAP library by KDAB.com" "http://www.kdab.com/kdsoap" TRUE "" "Need for access to the SugarCRM SOAP webservice. Required.")

if(KDSoap_CODEGENERATOR)
  # generate SOAP bindings for a wsdl file
  macro(kdsoap_generate_soap_bindings _sources _wsdl _name)
    add_custom_command(
      OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${_name}.h
      COMMAND ${KDSoap_CODEGENERATOR} -namespace KDSoapGenerated -o ${_name}.h
              ${_wsdl}
      DEPENDS ${_wsdl}
    )
    add_custom_command(
      OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${_name}.cpp
      COMMAND ${KDSoap_CODEGENERATOR} -o ${_name}.cpp -namespace KDSoapGenerated -impl ${_name}.h
              ${_wsdl}
      DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${_name}.h
              ${_wsdl}
    )

    qt4_generate_moc(${CMAKE_CURRENT_BINARY_DIR}/${_name}.h ${CMAKE_CURRENT_BINARY_DIR}/${_name}_moc.cpp)
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
configure_file(config-kwallet.h.in ${CMAKE_CURRENT_BINARY_DIR}/config-kwallet.h)

add_subdirectory(salesforce)
add_subdirectory(sugarcrm)
