set (_clientdir ${CMAKE_CURRENT_SOURCE_DIR}/../../../client )
include_directories ( ${_clientdir}/src/dialogs )

add_executable(resourceconfigdialogtest resourceconfigdialogtest.cpp)

target_link_libraries(resourceconfigdialogtest
    sugarclientprivate
	${QT_QTGUI_LIBS}
)

