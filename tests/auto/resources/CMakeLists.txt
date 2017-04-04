#auto tests

set(_resourcesdir ${CMAKE_CURRENT_SOURCE_DIR}/../../../resources)

include_directories(
  ${_resourcesdir}/sugarcrm
  ${CMAKE_CURRENT_SOURCE_DIR}/../../../kdcrmdata
  ${CMAKE_CURRENT_SOURCE_DIR}/../../..
)


kde4_add_library(sugar_mock_protocol_private sugarmockprotocol.cpp)

macro(add_resources_tests)
  foreach(_testname ${ARGN})
    kde4_add_unit_test(${_testname} TESTNAME "${_testname}" ${_testname}.cpp)
    target_link_libraries(${_testname}
      akonadi_sugarcrm_resource_private
      sugar_mock_protocol_private
      kdcrmdata
      ${QT_QTTEST_LIBRARY}
      ${QT_QTGUI_LIBS}
    )
  endforeach()
endmacro()

add_resources_tests(
  test_sugarsession
  test_sugarmockprotocol
)
