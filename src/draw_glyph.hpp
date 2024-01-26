#ifndef DRAW_GLYPH_HPP
#define DRAW_GLYPH_HPP

#include "font.hpp"

void DrawGlyph(SDL_Renderer *renderer, DebugSettings &debug, const Font &font,
               const Glyph &g, const SDL_Color &color, const int &x,
               const int &y);

void DrawGlyph(SDL_Renderer *renderer, DebugSettings &debug, const Font &font,
               const Glyph &g, const SDL_Color &color, const int &x,
               const int &y, const hb_glyph_position_t &hb_glyph_pos);

#endif