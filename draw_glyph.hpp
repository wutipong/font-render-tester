#pragma once
#include "font.hpp"

void InitDrawGlyph();

void DrawGlyph(const Glyph &glyph, const float &x, const float &y,
               const SDL_Color &color, const int &screenWidth,
               const int &screenHeight);

void CleanUpDrawGlyph();