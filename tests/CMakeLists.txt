file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS
        *.cpp
        *.hpp
)

add_executable(unit_tests ${TEST_SOURCES})

target_link_libraries(unit_tests
        PRIVATE
        gtest
        gtest_main
        gmock
        core
)

target_include_directories(unit_tests
        PRIVATE
        ${gtest_SOURCE_DIR}/include
        ${gmock_SOURCE_DIR}/include
)

include(GoogleTest)
gtest_discover_tests(unit_tests)