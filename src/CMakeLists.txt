cmake_minimum_required(VERSION 3.0.0)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

# Lib
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
    core/LineProvider.cpp
    core/LogFormat.cpp
    core/LogFormatIO.cpp
    core/LogFormatModel.cpp
    core/LogFormatStore.cpp
    core/LogModel.cpp
    core/MainController.cpp
    core/StdinLineProvider.cpp
    # ui
    ui/ColorWidget.cpp
    ui/HighlightWidget.cpp
    ui/HighlightWidget.ui
    ui/LogFormatDialog.cpp
    ui/LogFormatDialog.ui
    ui/MainWindow.cpp
    )

target_include_directories(lovilib PUBLIC
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/core
    ${CMAKE_CURRENT_SOURCE_DIR}/ui
    )

target_link_libraries(lovilib Qt5::Widgets)

# Main
add_executable(lovi ui/main.cpp)
target_link_libraries(lovi lovilib)

# Install
install(TARGETS ${PROJECT_NAME} DESTINATION bin)
