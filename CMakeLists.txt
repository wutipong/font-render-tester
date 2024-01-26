cmake_minimum_required(VERSION 3.19)

project(font-render-tester)

add_executable(font-render-tester
        "src/colors.hpp"
        "src/context.hpp"
        "src/draw_glyph.cpp"
        "src/draw_glyph.hpp"
        "src/font.cpp"
        "src/font.hpp"
        "src/io_util.hpp"
        "src/main_scene.cpp"
        "src/main_scene.hpp"
        "src/main.cpp"
        "src/scene.cpp"
        "src/scene.hpp"
        "src/settings.cpp"
        "src/settings.hpp"
        "src/text_renderer.cpp"
        "src/text_renderer.hpp"
        "src/texture.cpp"
        "src/texture.hpp"
)
target_include_directories(font-render-tester PRIVATE 
        "ext/imgui-filebrowser"
        "ext/IconFontCppHeaders"
)

target_compile_features(font-render-tester PRIVATE cxx_std_23)
set_property(TARGET font-render-tester PROPERTY CXX_STANDARD 23)
set_property(TARGET font-render-tester PROPERTY CXX_STANDARD_REQUIRED ON)

if (MSVC)
        set_property(TARGET font-render-tester PROPERTY WIN32_EXECUTABLE ON)
        target_compile_options(font-render-tester PRIVATE /Zc:__cplusplus)
endif ()

find_package(freetype CONFIG REQUIRED)
find_package(harfbuzz CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(magic_enum CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(utf8cpp CONFIG REQUIRED)

target_link_libraries(font-render-tester PRIVATE
        freetype 
        harfbuzz::harfbuzz
        imgui::imgui 
        magic_enum::magic_enum
        SDL2::SDL2 SDL2::SDL2main
        spdlog::spdlog spdlog::spdlog_header_only 
        utf8::cpp utf8cpp::utf8cpp 
)

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
        TARGET CopyFontFiles
        DESTINATION "fonts"
        INPUT 
                "fonts/forkawesome-webfont.ttf"
                "fonts/NotoSans-Regular.ttf"
                "fonts/NotoSansArabic-Regular.ttf"
                "fonts/NotoSansJP-Regular.ttf"
                "fonts/NotoSansKR-Regular.ttf"
                "fonts/NotoSansSC-Regular.ttf"
                "fonts/NotoSansTC-Regular.ttf"
                "fonts/NotoSansThai-Regular.ttf"
)