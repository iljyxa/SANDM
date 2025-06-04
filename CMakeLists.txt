cmake_minimum_required(VERSION 3.16)
project(SANDM LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui)

if(NOT Qt6_FOUND)
    message(FATAL_ERROR "Qt6 not found!")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(APP_VERSION_MAJOR 2)
set(APP_VERSION_MINOR 0)
set(APP_VERSION_PATCH 0)
set(APP_VERSION_SUFFIX "alpha")

include(FetchContent)

set(INSTALL_GTEST OFF)
set(BUILD_GMOCK ON CACHE BOOL "" FORCE)

FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/heads/main.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

FetchContent_MakeAvailable(googletest)

enable_testing()

find_package(Qt6 6.4 REQUIRED COMPONENTS Core Gui Widgets)
qt_standard_project_setup()

qt_add_resources(RESOURCES sandm.qrc)

add_subdirectory(core)
add_subdirectory(gui)
add_subdirectory(tests)

qt_add_executable(SANDM
        main.cpp
        ${RESOURCES}
)

target_link_libraries(SANDM
        PRIVATE
        core
        gui
        Qt::Core
        Qt::Gui
        Qt::Widgets
)

include(GNUInstallDirs)
install(TARGETS SANDM
        BUNDLE DESTINATION .
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
        TARGET SANDM
        OUTPUT_SCRIPT deploy_script
        NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})