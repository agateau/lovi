cmake_minimum_required(VERSION 3.0.0)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

# Lib
add_library(lovilib STATIC
    Color.cpp
    ColorWidget.cpp
    Conditions.cpp
    ConditionIO.cpp
    Config.cpp
    FileLineProvider.cpp
    FileWatcher.cpp
    Highlight.cpp
    HighlightModel.cpp
    HighlightWidget.cpp
    HighlightWidget.ui
    LineProvider.cpp
    LogFormat.cpp
    LogFormatDialog.cpp
    LogFormatDialog.ui
    LogFormatIO.cpp
    LogFormatModel.cpp
    LogFormatStore.cpp
    LogModel.cpp
    MainWindow.cpp
    StdinLineProvider.cpp
    )

target_include_directories(lovilib PUBLIC ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(lovilib Qt5::Widgets)

# Main
add_executable(lovi main.cpp)
target_link_libraries(lovi lovilib)

# Install
install(TARGETS ${PROJECT_NAME} DESTINATION bin)
