#auto tests

set(_clientdir ${CMAKE_CURRENT_SOURCE_DIR}/../../../client)

include_directories(
  ${_clientdir}/src/dialogs
  ${_clientdir}/src/models
  ${_clientdir}/src/utilities
  ${_clientdir}/src/widgets
  ${CMAKE_CURRENT_SOURCE_DIR}/../../../kdcrmdata
  ${CMAKE_CURRENT_SOURCE_DIR}/../../..
  ${CMAKE_BINARY_DIR}
)

macro(add_fatcrm_tests)
  foreach(_testname ${ARGN})
    ecm_add_test(${_testname}.cpp TEST_NAME "${_testname}" LINK_LIBRARIES
      fatcrmprivate
      kdcrmdata
      Qt5::Test
      Qt5::Gui
    )
  endforeach()
endmacro()

add_fatcrm_tests(
  referenceddatatest
  nullabledatecomboboxtest
  test_contactsimporter
  test_enumdefinitions
  test_accountrepository
  test_itemdataextractor
  test_linkeditemsrepository
  kdcrmutilstest
)
