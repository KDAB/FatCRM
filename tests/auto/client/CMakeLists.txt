#auto tests

set(_clientdir ${CMAKE_CURRENT_SOURCE_DIR}/../../../client)

include_directories(
  ${_clientdir}/src/dialogs
  ${_clientdir}/src/models
  ${_clientdir}/src/utilities
  ${_clientdir}/src/widgets
  ${CMAKE_CURRENT_SOURCE_DIR}/../../../kdcrmdata
  ${CMAKE_CURRENT_SOURCE_DIR}/../../..
)

macro(add_fatcrm_tests)
  foreach(_testname ${ARGN})
    kde4_add_unit_test(${_testname} TESTNAME "${_testname}" ${_testname}.cpp)
    target_link_libraries(${_testname}
      fatcrmprivate
      kdcrmdata
      Qt5::Test
      Qt5::Gui
    )
  endforeach()
endmacro()

add_fatcrm_tests(
  referenceddatatest
  qdateeditextest
  test_contactsimporter
  test_enumdefinitions
  test_accountcache
  test_accountrepository
  test_itemdataextractor
  kdcrmutilstest
)
