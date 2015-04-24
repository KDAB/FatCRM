include_directories( ${KDEPIMLIBS_INCLUDE_DIRS} ${KDE4_INCLUDES} ${QT_INCLUDES} ${Boost_INCLUDE_DIR} )

set (_clientdir ${CMAKE_CURRENT_SOURCE_DIR}/../../client )
include_directories(
	${_clientdir}/src/dialogs
	${_clientdir}/src/models
	${_clientdir}/src/utilities
	${CMAKE_CURRENT_SOURCE_DIR}/../../kdcrmdata
)

kde4_add_executable(referenceddatatest referenceddatatest.cpp)
target_link_libraries(referenceddatatest sugarclientprivate ${QT_QTGUI_LIBS} ${QT_QTTEST_LIBRARY})

