#pragma once
#include "font.hpp"

void DrawGlyph(Context &ctx, const Font &font, const Glyph &g,
               const SDL_Color &color, const float &x, const float &y);

void DrawGlyph(Context &ctx, const Font &font, const Glyph &g,
               const SDL_Color &color, const float &x, const float &y,
               const hb_glyph_position_t &hb_glyph_pos);