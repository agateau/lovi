cmake_minimum_required(VERSION 3.0.0)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

# Lib
add_library(lovilib STATIC
    conditions.cpp
    config.cpp
    filelineprovider.cpp
    filewatcher.cpp
    highlight.cpp
    lineprovider.cpp
    logformat.cpp
    logformatdialog.cpp
    logformatdialog.ui
    logformatloader.cpp
    logmodel.cpp
    mainwindow.cpp
    stdinlineprovider.cpp
    )

target_include_directories(lovilib PUBLIC ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(lovilib Qt5::Widgets)

# Main
add_executable(lovi main.cpp)
target_link_libraries(lovi lovilib)

# Install
install(TARGETS ${PROJECT_NAME} DESTINATION bin)
