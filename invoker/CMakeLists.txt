set(app_SRCS
    invoker.cpp
    main.cpp
)

kde4_add_executable(fatcrminvoker ${app_SRCS})

target_link_libraries(fatcrminvoker
  ${KDE4_KDECORE_LIBS}
  ${QT_QTDBUS_LIBRARY}
  ${QT_QTCORE_LIBRARY}
)

install(TARGETS fatcrminvoker ${INSTALL_TARGETS_DEFAULT_ARGS})

########### install files ###############

install(FILES fatcrminvoker.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
