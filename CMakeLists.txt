cmake_minimum_required(VERSION 3.17)

set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake
        CACHE STRING "Vcpkg toolchain file")

# set the project name
project(font-render-tester)

# add the executable
add_executable(font-render-tester
        "main.cpp"
        "scene.hpp" "scene.cpp"
        "imgui-filebrowser/imfilebrowser.h"
        "texture.hpp" "texture.cpp"
        "main_scene.hpp" "main_scene.cpp"
        "font.cpp" "font.hpp"
        "text_renderer.hpp" "text_renderer.cpp"
        "context.cpp"
        "draw_rect.cpp" "draw_glyph.hpp"
        "draw_glyph.cpp" "io_util.hpp"
        )

target_compile_features(font-render-tester PRIVATE cxx_std_17)
set_property(TARGET font-render-tester PROPERTY CXX_STANDARD 17)
set_property(TARGET font-render-tester PROPERTY CXX_STANDARD_REQUIRED ON)

if (MSVC)
    set_property(TARGET font-render-tester PROPERTY WIN32_EXECUTABLE ON)
    target_compile_options(font-render-tester PRIVATE /Zc:__cplusplus)
endif ()


find_package(SDL2 CONFIG REQUIRED)
find_package(freetype CONFIG REQUIRED)
find_package(harfbuzz CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(utf8cpp CONFIG REQUIRED)

target_include_directories(font-render-tester PRIVATE ${STB_INCLUDE_DIRS})

target_link_libraries(font-render-tester PRIVATE
        SDL2::SDL2 SDL2::SDL2main
        imgui::imgui harfbuzz::harfbuzz
        utf8cpp freetype spdlog::spdlog spdlog::spdlog_header_only)

function(copy_resources)
    set(oneValueArgs TARGET TARGET)
    set(multiValueArgs TARGET INPUT)
    set(oneValueArgs DESTINATION)

    cmake_parse_arguments(COPY_RESOURCES "${options}" "${oneValueArgs}"
            "${multiValueArgs}" ${ARGN}
            )

    file(GLOB COPY_RESOURCES_INPUT_FILES ${COPY_RESOURCES_INPUT})

    add_custom_target(${COPY_RESOURCES_TARGET} ALL
            COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:font-render-tester>/${COPY_RESOURCES_DESTINATION}/"
            COMMAND ${CMAKE_COMMAND} -E copy ${COPY_RESOURCES_INPUT_FILES} "$<TARGET_FILE_DIR:font-render-tester>/${COPY_RESOURCES_DESTINATION}/"
    )
endfunction()

copy_resources(
        TARGET CopyShaders
        DESTINATION "fonts"
        INPUT "fonts/NotoSans-Regular.ttf" "fonts/NotoSansThai-Regular.ttf" "fonts/NotoSansCJKjp-Regular.otf" "fonts/NotoSansCJKkr-Regular.otf" "fonts/NotoSansCJKsc-Regular.otf"
)