#pragma once
#include <functional>

#include "context.hpp"
#include "font.hpp"
#include <SDL2/SDL.h>
#include <harfbuzz/hb.h>

void TextRenderNoShape(Context &ctx, Font &font, const std::string &str,
                       const SDL_Color &color, const std::string &language,
                       const hb_script_t &script);

void TextRenderLeftToRight(Context &ctx, Font &font, const std::string &str,
                           const SDL_Color &color, const std::string &language,
                           const hb_script_t &script);

void TextRenderRightToLeft(Context &ctx, Font &font, const std::string &str,
                           const SDL_Color &color, const std::string &language,
                           const hb_script_t &script);

void TextRenderTopToBottom(Context &ctx, Font &font, const std::string &str,
                           const SDL_Color &color, const std::string &language,
                           const hb_script_t &script);
