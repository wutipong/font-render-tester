#pragma once
#include <functional>

#include <SDL2/SDL.h>
#include <harfbuzz/hb.h>
#include "font.hpp"

void TextRenderNoShape(SDL_Renderer *, const SDL_Rect &bound, Font &font,
                       const std::string &str, const SDL_Color &,
                       const hb_script_t &script);

void TextRenderLeftToRight(SDL_Renderer *, const SDL_Rect &bound, Font &font,
                           const std::string &str, const SDL_Color &,
                           const hb_script_t &script);

void TextRenderRightToLeft(SDL_Renderer *, const SDL_Rect &bound, Font &font,
                           const std::string &str, const SDL_Color &,
                           const hb_script_t &script);

void TextRenderTopToBottom(SDL_Renderer *, const SDL_Rect &bound, Font &font,
                           const std::string &str, const SDL_Color &,
                           const hb_script_t &script);

void DrawGlyph(SDL_Renderer *renderer, const Font& font, const Glyph &g, const SDL_Color &color,
               const int &x, const int &y);

void DrawGlyph(SDL_Renderer* renderer, const Font& font, const Glyph& g, const SDL_Color& color,
    const int& x, const int& y, const hb_glyph_position_t& hb_glyph_pos);