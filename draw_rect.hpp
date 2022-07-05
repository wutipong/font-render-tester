#pragma once
#include <glm/glm.hpp>

void InitDrawRect();
void CleanUpDrawRect();

enum class DrawRectMode
{
	Outline, Fill
};

void DrawRect(const float &x, const float &y, const float &w, const float &h,
              const glm::vec4 &color, const int &screenWidth,
              const int &screenHeight, const DrawRectMode& mode = DrawRectMode::Outline);
