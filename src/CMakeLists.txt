include(translations)
include(ECMAddAppIcon)

add_subdirectory(ui/appicon)

# Sources
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/BuildConfig.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/BuildConfig.h
)

set(APPLIB_SRCS
    # core
    core/Color.cpp
    core/ConditionIO.cpp
    core/Conditions.cpp
    core/Config.cpp
    core/FileLineProvider.cpp
    core/FileWatcher.cpp
    core/Filter.cpp
    core/FilterModel.cpp
    core/FilterProxyModel.cpp
    core/Highlight.cpp
    core/HighlightModel.cpp
    core/JsonSettings.cpp
    core/LineProvider.cpp
    core/LogFormat.cpp
    core/LogFormatIO.cpp
    core/LogFormatModel.cpp
    core/LogFormatStore.cpp
    core/LogModel.cpp
    core/MainController.cpp
    core/Searcher.cpp
    core/StdinLineProvider.cpp
    # ui
    ui/ColorWidget.cpp
    ui/ColorUtils.cpp
    ui/ConditionLineEdit.cpp
    ui/ConditionLineEditChecker.cpp
    ui/HighlightWidget.cpp
    ui/HighlightWidget.ui
    ui/ItemDelegate.cpp
    ui/LineEditChecker.cpp
    ui/LogFormatWidget.cpp
    ui/LogFormatWidget.ui
    ui/MainWindow.cpp
    ui/MainWindow.ui
    ui/Resources.cpp
    ui/ThinPlainTextEdit.cpp
    ui/WidgetFloater.cpp
    ui/WidgetUtils.cpp
)

qt5_add_resources(APPLIB_RESOURCES_SRCS ui/lovi.qrc)

# App library
add_library(${APPLIB_NAME} STATIC
    ${APPLIB_SRCS}
    ${QPROPGEN_SRCS}
    ${APPLIB_RESOURCES_SRCS}
    ${TRANSLATIONS_RESOURCES_SRCS}
)
target_include_directories(${APPLIB_NAME} PUBLIC
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}/core
    ${CMAKE_CURRENT_SOURCE_DIR}/core
    ${CMAKE_CURRENT_SOURCE_DIR}/ui
)
target_link_libraries(${APPLIB_NAME}
    Qt5::Core
    Qt5::Widgets
    KF5::GuiAddons
)

# Translations
set(TS_FILES
    translations/${APP_NAME}_en.ts
)
qt5_add_translation(QM_FILES ${TS_FILES})
add_custom_target(build_qm DEPENDS ${QM_FILES})
add_dependencies(${APPLIB_NAME} build_qm)

add_lupdate_target(SOURCES ${APPLIB_SRCS} TS_FILES ${TS_FILES} OPTIONS -no-obsolete)

# App executable
set(APP_SRCS ui/main.cpp)
ecm_add_app_icon(
    APP_SRCS
    ICONS
        ui/appicon/256-apps-${APP_NAME}.png
        ui/appicon/sc-apps-${APP_NAME}.svg
)
add_executable(${APP_NAME} WIN32 MACOSX_BUNDLE ${APP_SRCS})

if (APPLE)
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/macos/Info.plist.in
        ${CMAKE_CURRENT_BINARY_DIR}/macos/Info.plist
    )
    set_target_properties(${APP_NAME} PROPERTIES
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_BINARY_DIR}/macos/Info.plist
    )
endif()

target_link_libraries(${APP_NAME}
    ${APPLIB_NAME}
)

# Install
install(
    TARGETS ${APP_NAME}
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${BIN_INSTALL_DIR}
)

install(
    FILES ${QM_FILES}
    DESTINATION ${DATA_INSTALL_DIR}/translations
)

if (UNIX AND NOT APPLE)
    install(FILES linux/${APP_NAME}.desktop
        DESTINATION share/applications
        RENAME ${INVERSE_ORGANIZATION_NAME}.${APP_NAME}.desktop
    )
endif()

if (WIN32)
    include(DeployQt)
    windeployqt(${APP_NAME})
    # install kf5guiaddons.dll so that it is included in the dll
    get_target_property(kf5guiaddons_dll KF5::GuiAddons LOCATION)
    install(
        FILES ${kf5guiaddons_dll}
        DESTINATION ${BIN_INSTALL_DIR}
    )
endif()

if (APPLE)
    include(DeployQt)
    macdeployqt(${APP_NAME})
endif()
