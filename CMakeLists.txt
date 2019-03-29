cmake_minimum_required(VERSION 3.0.0)

project(lovi VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 17)

find_package(Qt5 REQUIRED Widgets)

add_executable(${PROJECT_NAME}
    "main.cpp"
    "conditions.cpp"
    "config.cpp"
    "lineprovider.cpp"
    "logmodel.cpp"
    "mainwindow.cpp"
    )

target_link_libraries(${PROJECT_NAME}
    Qt5::Widgets
    )
