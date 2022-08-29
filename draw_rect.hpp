#pragma once

#include <SDL.h>

enum class DrawRectMode { Outline, Fill };

void DrawRect(SDL_Renderer *renderer, const float &x, const float &y,
              const float &w, const float &h, const SDL_Color &color,
              const int &screenWidth, const int &screenHeight,
              const DrawRectMode &mode = DrawRectMode::Outline);

void DrawLine(SDL_Renderer *renderer, const float &x1, const float &y1,
              const float &x2, const float &y2, const SDL_Color &color,
              const int &screenWidth, const int &screenHeight);