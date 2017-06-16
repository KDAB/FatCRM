#auto tests

set(_resourcesdir ${CMAKE_CURRENT_SOURCE_DIR}/../..)

include_directories(
  ${_resourcesdir}/sugarcrm
  ${CMAKE_CURRENT_BINARY_DIR}/..
  ${CMAKE_CURRENT_SOURCE_DIR}/../../../kdcrmdata
  ${CMAKE_CURRENT_SOURCE_DIR}/../../..
)


macro(add_resources_tests)
  foreach(_testname ${ARGN})
    kde4_add_unit_test(${_testname} TESTNAME "${_testname}" ${_testname}.cpp)
    target_link_libraries(${_testname}
      akonadi_sugarcrm_resource_private
      kdcrmdata
      ${QT_QTTEST_LIBRARY}
      ${QT_QTGUI_LIBS}
    )
  endforeach()
endmacro()

add_resources_tests(
  test_sugarsession
  test_sugarmockprotocol
  test_loginjob
  test_listentriesjob
  test_createentryjob
  test_updateentryjob
  test_fetchentryjob
  test_deleteentryjob
  test_jobwithsugarsoapprotocol
)

add_dependencies(test_listentriesjob generate_sugarsoap_h)
