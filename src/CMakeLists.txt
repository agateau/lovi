cmake_minimum_required(VERSION 3.0.0)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

# Lib
qt5_add_resources(lovi_resources ui/lovi.qrc)

qpropgen(lovilib_qpropgen_sources
    core/BaseMainController.yaml
    )

add_library(lovilib STATIC
    # core
    core/Color.cpp
    core/ConditionIO.cpp
    core/Conditions.cpp
    core/Config.cpp
    core/FileLineProvider.cpp
    core/FileWatcher.cpp
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
    ${lovilib_qpropgen_sources}
    # ui
    ui/ColorWidget.cpp
    ui/ColorUtils.cpp
    ui/ConditionLineEditChecker.cpp
    ui/HighlightWidget.cpp
    ui/HighlightWidget.ui
    ui/ItemDelegate.cpp
    ui/LineEditChecker.cpp
    ui/LogFormatWidget.cpp
    ui/LogFormatWidget.ui
    ui/MainWindow.cpp
    ui/MainWindow.ui
    ui/WidgetFloater.cpp
    ${lovi_resources}
    )

target_include_directories(lovilib PUBLIC
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}/core
    ${CMAKE_CURRENT_SOURCE_DIR}/core
    ${CMAKE_CURRENT_SOURCE_DIR}/ui
    )

target_link_libraries(lovilib Qt5::Widgets)

# Main
add_executable(lovi ui/main.cpp)
target_link_libraries(lovi lovilib)

# Install
install(TARGETS ${PROJECT_NAME} DESTINATION bin)

install(FILES ui/icons/lovi.desktop
    DESTINATION share/applications
    RENAME com.agateau.lovi.desktop
)

install(FILES ui/icons/appicon.svg
    DESTINATION share/icons/hicolor/scalable/apps
    RENAME lovi.svg
)
