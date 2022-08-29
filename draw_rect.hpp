#pragma once

#include "context.hpp"
#include <SDL.h>

enum class DrawRectMode { Outline, Fill };

void DrawRect(Context &ctx, const float &x, const float &y, const float &w,
              const float &h, const SDL_Color &color,
              const DrawRectMode &mode = DrawRectMode::Outline);

void DrawLine(Context &ctx, const float &x1, const float &y1, const float &x2,
              const float &y2, const SDL_Color &color);