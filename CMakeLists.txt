cmake_minimum_required(VERSION 3.16)
project(SANDM LANGUAGES CXX)
include(version.cmake)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui)

if(NOT Qt6_FOUND)
    message(FATAL_ERROR "Qt6 not found!")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/version.hpp.in
        ${CMAKE_CURRENT_SOURCE_DIR}/version.hpp
)

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

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Gui Widgets)
qt_standard_project_setup()

qt_add_resources(RESOURCES sandm.qrc)

add_subdirectory(core)
add_subdirectory(gui)
add_subdirectory(tests)

if(WIN32)
    set(APP_ICON "${CMAKE_CURRENT_SOURCE_DIR}/icons/sandm.ico")
elseif(APPLE)
    set(APP_ICON "${CMAKE_CURRENT_SOURCE_DIR}/icons/sandm.icns")
endif()

qt_add_executable(SANDM
        WIN32 MACOSX_BUNDLE
        main.cpp
        ${RESOURCES}
)

if(WIN32 AND EXISTS "${APP_ICON}")
    set_target_properties(SANDM PROPERTIES
            RC_ICONS "${APP_ICON}"
    )
elseif(APPLE AND EXISTS "${APP_ICON}")
    set_target_properties(SANDM PROPERTIES
            MACOSX_BUNDLE_ICON_FILE "sandm.icns"
    )
    set_source_files_properties("${APP_ICON}" PROPERTIES
            MACOSX_PACKAGE_LOCATION "Resources"
    )
    target_sources(SANDM PRIVATE "${APP_ICON}")
endif()

target_include_directories(SANDM PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

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

if (WIN32 AND MINGW)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-subsystem,windows")
endif()

qt_generate_deploy_app_script(
        TARGET SANDM
        OUTPUT_SCRIPT deploy_script
        NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})