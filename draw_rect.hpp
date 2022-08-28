#pragma once
#include <SDL.h>

void InitDrawRect();
void CleanUpDrawRect();

enum class DrawRectMode { Outline, Fill };

void DrawRect(const float &x, const float &y, const float &w, const float &h,
              const SDL_Color &color, const int &screenWidth,
              const int &screenHeight,
              const DrawRectMode &mode = DrawRectMode::Outline);
