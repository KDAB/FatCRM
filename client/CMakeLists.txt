find_package(KDReports REQUIRED)
set_package_properties(KDReports PROPERTIES
  URL "https://github.com/KDAB/KDReports"
  DESCRIPTION "Qt library for generating printable and exportable reports from code and from XML descriptions"
  TYPE REQUIRED
  PURPOSE "Required to generate reports."
)

include_directories(${KDReports_INCLUDE_DIR}
    ${KDReports_INCLUDE_DIR}/KDReports
)

if(PhoneNumber_FOUND AND ICU_FOUND)
    include_directories(${PhoneNumber_INCLUDE_DIR}
        ${ICU_INCLUDE_DIRS}
    )
endif()

add_subdirectory(src)
add_subdirectory(icons)
