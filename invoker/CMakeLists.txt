add_executable(fatcrminvoker
    invoker.cpp
    main.cpp
)
target_link_libraries(fatcrminvoker PRIVATE
  Qt5::DBus
  Qt5::Core
  KF5::Service
)

install(TARGETS fatcrminvoker ${INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES fatcrminvoker.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
