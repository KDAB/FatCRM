
# To ensure KDSoap is found, add its base directory to CMAKE_PREFIX_PATH
find_package(KDSoap CONFIG) # first try KDSoapConfig.cmake
find_package(KDSoap REQUIRED) # then try FindKDSoap.cmake
macro_log_feature(KDSoap "KD SOAP" "Qt SOAP library by KDAB.com" "http://www.kdab.com/kdsoap" TRUE "" "Need for access to the SugarCRM SOAP webservice. Required.")


add_subdirectory( salesforce )
add_subdirectory( sugarcrm )
