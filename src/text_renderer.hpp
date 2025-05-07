#pragma once

#include "debug_settings.hpp"
#include "font.hpp"
#include <SDL3/SDL.h>
#include <functional>
#include <harfbuzz/hb.h>

enum class TextDirection {
  LeftToRight,
  TopToBottom,
#ifdef ENABLE_RTL
  RightToLeft,
#endif
};

void TextRenderNoShape(SDL_Renderer *renderer, DebugSettings &debug, Font &font,
                       const std::string &str, const SDL_Color &color);

void TextRenderLeftToRight(SDL_Renderer *renderer, DebugSettings &debug,
                           Font &font, const std::string &str,
                           const SDL_Color &color, const std::string &language,
                           const hb_script_t &script);

void TextRenderTopToBottom(SDL_Renderer *renderer, DebugSettings &debug,
                           Font &font, const std::string &str,
                           const SDL_Color &color, const std::string &language,
                           const hb_script_t &script);

#ifdef ENABLE_RTL

void TextRenderRightToLeft(SDL_Renderer *renderer, Context &debug, Font &font,
                           const std::string &str, const SDL_Color &color,
                           const std::string &language,
                           const hb_script_t &script);
#endif
