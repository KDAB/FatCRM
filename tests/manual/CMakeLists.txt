include_directories( ${KDEPIMLIBS_INCLUDE_DIRS} ${KDE4_INCLUDES} ${QT_INCLUDES} ${Boost_INCLUDE_DIR} )

set (_clientdir ${CMAKE_CURRENT_SOURCE_DIR}/../../client )
include_directories(
	${_clientdir}/src/dialogs
	${_clientdir}/src/models
	${_clientdir}/src/utilities
	${CMAKE_CURRENT_SOURCE_DIR}/../../kdcrmdata
)

add_executable(resourceconfigdialogtest resourceconfigdialogtest.cpp)
target_link_libraries(resourceconfigdialogtest sugarclientprivate ${QT_QTGUI_LIBS})

add_executable(itemstreemodeltest itemstreemodeltest.cpp)
target_link_libraries(itemstreemodeltest sugarclientprivate ${QT_QTGUI_LIBS})

