#include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/.. )
set(RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/../../../kdcrmdata
)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})

#use these variables to control the Akonadi-based tests. by default all backends are run
#set(AKONADI_RUN_SQLITE_ISOLATED_TESTS TRUE)
#set(AKONADI_RUN_MYSQL_ISOLATED_TESTS FALSE)

#find the akonaditest that matches the version we are building with
set(CMAKE_PROGRAM_PATH "${AKONADI_INSTALL_DIR}/bin")

add_akonadi_isolated_test(test_sugarcrmresource.cpp)

target_link_libraries(test_sugarcrmresource
    kdcrmdata
)
