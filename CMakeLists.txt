cmake_minimum_required(VERSION 3.16)

project(SANDM LANGUAGES CXX)

set(CMAKE_PREFIX_PATH "C:/Qt/6.8.2/mingw_64")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static -lpthread")

set(APP_VERSION_MAJOR 0)
set(APP_VERSION_MINOR 0)
set(APP_VERSION_PATCH 1)
set(APP_VERSION_SUFFIX "alpha")

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Gui Widgets)

qt_standard_project_setup()

qt_add_resources(RESOURCES sandm.qrc)

qt_add_executable(SANDM main.cpp
        core/include/memory_manager.hpp core/src/memory_manager.cpp
        core/include/processor.hpp core/src/processor.cpp
        core/include/processor_observer.hpp
        core/src/assembler.cpp core/include/assembler.hpp
        core/include/bytes.hpp
        core/include/common_definitions.hpp
        gui/src/main_window.cpp
        gui/include/main_window.hpp
        gui/include/code_editor.hpp
        gui/include/syntax_highlighter.hpp
        gui/src/code_editor.cpp
        gui/include/console.hpp
        gui/include/register_editor.hpp
        gui/src/register_editor.cpp
        gui/include/hex_spin_box.hpp
        gui/include/memory_model.hpp
        gui/include/memory_view.hpp
        core/include/processor_io.hpp
        core/include/virtual_machine.hpp
        gui/include/virtual_machine_controller.hpp
        ${RESOURCES}
        gui/include/main_window.hpp
        gui/src/virtual_machine_controller.cpp
        core/src/virtual_machine.cpp
        version.hpp
        gui/include/style_colors.hpp
)

target_link_libraries(SANDM
        PRIVATE
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

if (WIN32 AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(DEBUG_SUFFIX)
    if (MSVC AND CMAKE_BUILD_TYPE MATCHES "Debug")
        set(DEBUG_SUFFIX "d")
    endif ()
    set(QT_INSTALL_PATH "${CMAKE_PREFIX_PATH}")
    if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
        set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
            set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        endif ()
    endif ()
    if (EXISTS "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
    endif ()
    foreach (QT_LIB Core Gui Widgets)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/bin/Qt6${QT_LIB}${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
    endforeach (QT_LIB)
endif ()