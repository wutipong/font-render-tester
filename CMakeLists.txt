cmake_minimum_required(VERSION 3.17)

# set the project name
project(font-render-tester)

# add the executable
add_executable(font-render-tester 
	"main.cpp" 
	"scene.hpp" "scene.cpp" 
	"imgui_impl_sdl.h" "imgui_impl_sdl.cpp"
	"imgui_impl_opengl3.h" "imgui_impl_opengl3.cpp"
	"imgui-filebrowser/imfilebrowser.h"
	"texture.hpp" "texture.cpp" 
	"main_scene.hpp" "main_scene.cpp" 
	"font.cpp" "font.hpp" 
	"text_renderer.hpp" "text_renderer.cpp" 
	"context.cpp" "test_scene.cpp" 
	"draw_rect.cpp" "draw_glyph.hpp" 
	"draw_glyph.cpp" "io_util.hpp")

target_compile_features(font-render-tester PRIVATE cxx_std_17)
set_property(TARGET font-render-tester PROPERTY CXX_STANDARD 17)
set_property(TARGET font-render-tester PROPERTY CXX_STANDARD_REQUIRED ON)

if (MSVC)
	target_compile_options(font-render-tester PRIVATE /Zc:__cplusplus)
endif()


find_package(SDL2 CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(harfbuzz CONFIG REQUIRED)
find_package(utf8cpp CONFIG REQUIRED)
find_package(gl3w CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(freetype CONFIG REQUIRED)

target_include_directories(font-render-tester PRIVATE ${STB_INCLUDE_DIRS})

target_link_libraries(font-render-tester PRIVATE SDL2::SDL2 SDL2::SDL2main imgui::imgui harfbuzz::harfbuzz utf8cpp unofficial::gl3w::gl3w freetype)

file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/fonts/NotoSans-Regular.ttf" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/fonts")
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/fonts/NotoSansThai-Regular.ttf" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/fonts")
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/fonts/NotoSansCJKjp-Regular.otf" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/fonts")
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/fonts/NotoSansCJKkr-Regular.otf" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/fonts")
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/fonts/NotoSansCJKsc-Regular.otf" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/fonts")

file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/shaders/draw_rect.vert" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/shaders")
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/shaders/draw_rect.frag" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/shaders")
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/shaders/draw_glyph.vert" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/shaders")
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/shaders/draw_glyph.frag" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/shaders")