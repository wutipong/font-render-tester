#pragma once
#include <functional>

#include "context.hpp"
#include "font.hpp"
#include <SDL2/SDL.h>
#include <harfbuzz/hb.h>

void TextRenderNoShape(Context &ctx, Font &font, const std::string &str,
                       const SDL_Color &, const hb_script_t &script);

void TextRenderLeftToRight(Context &ctx, Font &font, const std::string &str,
                           const SDL_Color &, const hb_script_t &script);

void TextRenderRightToLeft(Context &ctx, Font &font, const std::string &str,
                           const SDL_Color &, const hb_script_t &script);

void TextRenderTopToBottom(Context &ctx, Font &font, const std::string &str,
                           const SDL_Color &, const hb_script_t &script);

void DrawGlyph(SDL_Renderer *renderer, const Font &font, const Glyph &g,
               const SDL_Color &color, const int &x, const int &y);

void DrawGlyph(SDL_Renderer *renderer, const Font &font, const Glyph &g,
               const SDL_Color &color, const int &x, const int &y,
               const hb_glyph_position_t &hb_glyph_pos);
