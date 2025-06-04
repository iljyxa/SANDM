qt_add_library(gui STATIC
        include/gui/main_window.hpp src/main_window.cpp
        include/gui/code_editor.hpp src/code_editor.cpp
        include/gui/register_editor.hpp src/register_editor.cpp
        include/gui/console.hpp
        include/gui/syntax_highlighter.hpp
        include/gui/hex_spin_box.hpp
        include/gui/spin_box_hoverable.hpp
        include/gui/memory_model.hpp
        include/gui/memory_view.hpp
        include/gui/virtual_machine_controller.hpp src/virtual_machine_controller.cpp
        include/gui/style_colors.hpp
)

target_include_directories(gui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(gui
        PUBLIC Qt::Core Qt::Gui Qt::Widgets
        PRIVATE core
)
