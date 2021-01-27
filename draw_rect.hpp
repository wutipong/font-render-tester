#pragma once
#include <glm/glm.hpp>

void InitDrawRect();
void CleanUpDrawRect();

void DrawRect(const float& x, const float& y, const float& w, const float& h, const glm::vec4& color, const int& screenWidth, const int& screenHeight);
