file(GLOB_RECURSE CORE_SOURCES CONFIGURE_DEPENDS
        src/*.cpp
)

add_library(core STATIC ${CORE_SOURCES})
target_include_directories(core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)