set(TEST_DATA_DIR ${CMAKE_CURRENT_SOURCE_DIR})
configure_file(
    testconfig.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/testconfig.h)

add_executable(tests
    conditioniotest.cpp
    filewatchertest.cpp
    logformatiotest.cpp
    tests.cpp
    testutils.cpp
)

target_include_directories(tests PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(tests
    lovilib
    Qt5::Test
    Catch2::Catch2
)

add_test(NAME tests COMMAND tests)
