#pragma once
#include "font.hpp"
#include <glm/glm.hpp>

void InitDrawGlyph();

void DrawGlyph(const Glyph& glyph, const float& x, const float& y, const glm::vec4& color, const int& screenWidth,
    const int& screenHeight);

void CleanUpDrawGlyph();