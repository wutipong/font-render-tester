#pragma once
#include <functional>

#include "font.hpp"
#include <SDL2/SDL.h>
#include <harfbuzz/hb.h>

enum class TextRendererEnum { None, NoShape, LeftToRight, TopToBottom };

void TextRenderNoShape(SDL_Renderer *, const SDL_Rect &bound, Font &font,
                       const std::string &str, const SDL_Color &,
                       const hb_script_t &script);

void TextRenderLeftToRight(SDL_Renderer *, const SDL_Rect &bound, Font &font,
                           const std::string &str, const SDL_Color &,
                           const hb_script_t &script);

void TextRenderTopToBottom(SDL_Renderer *, const SDL_Rect &bound, Font &font,
                           const std::string &str, const SDL_Color &,
                           const hb_script_t &script);
