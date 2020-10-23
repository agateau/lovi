add_custom_target(check
    COMMAND ${CMAKE_CTEST_COMMAND} --verbose --build-config "$<CONFIG>"
)

set(TEST_DATA_DIR ${CMAKE_CURRENT_SOURCE_DIR})
configure_file(
    testconfig.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/testconfig.h)

# Unit test
add_executable(tests
    ConditionIOTest.cpp
    FileLineProviderTest.cpp
    FileWatcherTest.cpp
    JsonSettingsTest.cpp
    LogFormatIOTest.cpp
    LogFormatStoreTest.cpp
    LogFormatTest.cpp
    SearcherTest.cpp
    tests.cpp
    Catch2QtUtils.cpp
)

target_include_directories(tests PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(tests
    ${APPLIB_NAME}
    Qt5::Test
    Catch2::Catch2
)

add_test(NAME tests COMMAND tests)

# Before running tests, make sure tests they are built if necessary
add_dependencies(check tests)

# Color contrast table generator
add_executable(colortablegen
    ColorTableGen.cpp
)

target_link_libraries(colortablegen ${APPLIB_NAME})
