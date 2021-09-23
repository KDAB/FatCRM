#manual tests

set(_clientdir ${CMAKE_CURRENT_SOURCE_DIR}/../../client)

include_directories(
  ${CMAKE_BINARY_DIR}
  ${_clientdir}/src/dialogs
  ${_clientdir}/src/models
  ${_clientdir}/src/utilities
  ${CMAKE_CURRENT_SOURCE_DIR}/../../kdcrmdata
)

add_executable(resourceconfigdialogtest resourceconfigdialogtest.cpp)
target_link_libraries(resourceconfigdialogtest
  fatcrmprivate
  Qt5::Gui
  KF5::AkonadiCore
  Qt5::Widgets
  kdcrmdata
)

add_executable(itemstreemodeltest itemstreemodeltest.cpp)
target_link_libraries(itemstreemodeltest
  fatcrmprivate
  Qt5::Gui
  KF5::AkonadiWidgets
  Qt5::Widgets
  kdcrmdata
)
