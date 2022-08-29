#pragma once
#include "font.hpp"

void DrawGlyph(SDL_Renderer *renderer, const Glyph &glyph, const float &x, const float &y,
               const SDL_Color &color, const int &screenWidth,
               const int &screenHeight);