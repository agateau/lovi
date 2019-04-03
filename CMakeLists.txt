cmake_minimum_required(VERSION 3.0.0)

project(lovi VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_CXX_STANDARD 17)

find_package(Qt5 REQUIRED Widgets)

add_executable(${PROJECT_NAME}
    "main.cpp"
    "conditions.cpp"
    "filelineprovider.cpp"
    "filewatcher.cpp"
    "highlight.cpp"
    "lineprovider.cpp"
    "logformat.cpp"
    "logformatdialog.cpp"
    "logformatloader.cpp"
    "logmodel.cpp"
    "mainwindow.cpp"
    )

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(${PROJECT_NAME}
    Qt5::Widgets
    )
