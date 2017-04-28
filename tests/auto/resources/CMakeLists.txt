#auto tests

set(_resourcesdir ${CMAKE_CURRENT_SOURCE_DIR}/../../../resources)

include_directories(
  ${_resourcesdir}/sugarcrm
  ${CMAKE_CURRENT_BINARY_DIR}/../../../resources/sugarcrm
  ${CMAKE_CURRENT_SOURCE_DIR}/../../../kdcrmdata
  ${CMAKE_CURRENT_SOURCE_DIR}/../../..
)


add_library(sugar_mock_protocol_private STATIC sugarmockprotocol.cpp)
target_link_libraries(sugar_mock_protocol_private
   KF5::CoreAddons
   Qt5::Core
)

macro(add_resources_tests)
  foreach(_testname ${ARGN})
    ecm_add_test(${_testname}.cpp TEST_NAME "${_testname}" LINK_LIBRARIES
      sugar_mock_protocol_private
      akonadi_sugarcrm_resource_private
      kdcrmdata
      Qt5::Test
      Qt5::Gui
    )
  endforeach()
endmacro()

add_resources_tests(
  test_sugarsession
  test_sugarmockprotocol
  test_loginjob
  test_listentriesjob
)
