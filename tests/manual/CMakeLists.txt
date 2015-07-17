#manual tests

set(_clientdir ${CMAKE_CURRENT_SOURCE_DIR}/../../client)

include_directories(
  ${_clientdir}/src/dialogs
  ${_clientdir}/src/models
  ${_clientdir}/src/utilities
  ${CMAKE_CURRENT_SOURCE_DIR}/../../kdcrmdata
)

add_executable(resourceconfigdialogtest resourceconfigdialogtest.cpp)
target_link_libraries(resourceconfigdialogtest
  fatcrmprivate
  ${QT_QTGUI_LIBS}
)

kde4_add_executable(itemstreemodeltest itemstreemodeltest.cpp)
target_link_libraries(itemstreemodeltest
  fatcrmprivate
  ${QT_QTGUI_LIBS}
)
