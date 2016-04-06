find_package(KDReports REQUIRED)
set_package_properties(KDReports PROPERTIES
  URL "https://github.com/KDAB/KDReports"
  DESCRIPTION "Qt library for generating printable and exportable reports from code and from XML descriptions"
  TYPE REQUIRED
  PURPOSE "Required to generate reports."
)

add_subdirectory(src)
add_subdirectory(icons)
