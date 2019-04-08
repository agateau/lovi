add_executable(tests
    tests.cpp
    filewatchertest.cpp
    testutils.cpp
)

target_link_libraries(tests
    lovilib
    Qt5::Test
    Catch2::Catch2
)

add_test(NAME tests COMMAND tests)
