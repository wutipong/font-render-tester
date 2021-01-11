#pragma once
#include <functional>

#include <SDL2/SDL.h>
#include <harfbuzz/hb.h>
#include "font.hpp"

enum class TextRendererEnum {
    None,
    NoShape,
    LeftToRight
};

void TextRenderNoShape(SDL_Renderer *, const SDL_Rect &bound, Font &font,
                       const std::string &str, const SDL_Color &,
                       const hb_script_t &script);

void TextRenderLeftToRight(SDL_Renderer *, const SDL_Rect &bound, Font &font,
                           const std::string &str, const SDL_Color &,
                           const hb_script_t &script);
