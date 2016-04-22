#auto tests

set(_clientdir ${CMAKE_CURRENT_SOURCE_DIR}/../../client)

include_directories(
  ${_clientdir}/src/dialogs
  ${_clientdir}/src/models
  ${_clientdir}/src/utilities
  ${_clientdir}/src/widgets
  ${CMAKE_CURRENT_SOURCE_DIR}/../../kdcrmdata
  ${CMAKE_CURRENT_SOURCE_DIR}/../..
)

macro(add_fatcrm_tests)
    foreach(_testname ${ARGN})
       set(_test ${_testname})
       get_filename_component(_name ${_testname} NAME_WE)
       ecm_add_test(${_testname} 
        TEST_NAME ${_testname}
        NAME_PREFIX "fatcrm-"
        LINK_LIBRARIES fatcrmprivate Qt5::Test kdcrmdata )
    endforeach()
endmacro ()

add_fatcrm_tests(
  referenceddatatest
  qdateeditextest
  test_contactsimporter
  test_enumdefinitions
  test_accountcache
  kdcrmutilstest
)
