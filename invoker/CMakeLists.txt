set(app_SRCS
    invoker.cpp
    main.cpp
)

add_executable(fatcrminvoker ${app_SRCS})

target_link_libraries(fatcrminvoker
  Qt5::DBus
  Qt5::Core
  KF5::Service
)

install(TARGETS fatcrminvoker ${INSTALL_TARGETS_DEFAULT_ARGS})

########### install files ###############

install(FILES fatcrminvoker.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
