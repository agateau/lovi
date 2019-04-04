cmake_minimum_required(VERSION 3.0.0)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

add_executable(${PROJECT_NAME}
    main.cpp
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
    )

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(${PROJECT_NAME}
    Qt5::Widgets
    )

# Install
install(TARGETS ${PROJECT_NAME} DESTINATION bin)
