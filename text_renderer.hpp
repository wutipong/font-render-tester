#pragma once

#include "context.hpp"
#include "font.hpp"
#include <SDL2/SDL.h>
#include <functional>
#include <harfbuzz/hb.h>

enum class TextDirection {
  LeftToRight,
  RightToLeft,
  TopToBottom,
};

void TextRenderNoShape(SDL_Renderer *renderer, Context &ctx, Font &font,
                       const std::string &str, const SDL_Color &color);

void TextRenderLeftToRight(SDL_Renderer *renderer, Context &ctx, Font &font,
                           const std::string &str, const SDL_Color &color,
                           const std::string &language,
                           const hb_script_t &script);

void TextRenderRightToLeft(SDL_Renderer *renderer, Context &ctx, Font &font,
                           const std::string &str, const SDL_Color &color,
                           const std::string &language,
                           const hb_script_t &script);

void TextRenderTopToBottom(SDL_Renderer *renderer, Context &ctx, Font &font,
                           const std::string &str, const SDL_Color &color,
                           const std::string &language,
                           const hb_script_t &script);
