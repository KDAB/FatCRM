#include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/.. )
set( RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} )

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/../../../kdcrmdata
)

set(testtools_SRCS
    kwaitforsignal.cpp
)

kde4_add_library(testtools ${testtools_SRCS})

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})
include(AkonadiMacros)

set(KDEPIMLIBS_RUN_ISOLATED_TESTS TRUE)
set(KDEPIMLIBS_RUN_SQLITE_ISOLATED_TESTS TRUE)

add_akonadi_isolated_test(test_sugarcrmresource.cpp)

target_link_libraries(test_sugarcrmresource
    kdcrmdata
    testtools
)
