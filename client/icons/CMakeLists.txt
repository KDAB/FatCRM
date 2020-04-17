set(fatcrm_ICONS
    ${CMAKE_CURRENT_SOURCE_DIR}/128-apps-fatcrm.png
    ${CMAKE_CURRENT_SOURCE_DIR}/16-apps-fatcrm.png
    ${CMAKE_CURRENT_SOURCE_DIR}/22-apps-fatcrm.png
    ${CMAKE_CURRENT_SOURCE_DIR}/32-apps-fatcrm.png
    ${CMAKE_CURRENT_SOURCE_DIR}/48-apps-fatcrm.png
    ${CMAKE_CURRENT_SOURCE_DIR}/64-apps-fatcrm.png
    PARENT_SCOPE
)

ecm_install_icons(ICONS
    ${fatcrm_ICONS}
    16-apps-sugarclient.png
    32-apps-sugarclient.png
    48-apps-sugarclient.png
    DESTINATION ${KDE_INSTALL_ICONDIR} THEME hicolor )
