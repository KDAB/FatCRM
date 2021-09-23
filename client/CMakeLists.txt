if(PhoneNumber_FOUND AND ICU_FOUND)
    include_directories(${PhoneNumber_INCLUDE_DIR}
        ${ICU_INCLUDE_DIRS}
    )
endif()

add_subdirectory(icons) # gives us fatcrm_ICONS
add_subdirectory(src)
