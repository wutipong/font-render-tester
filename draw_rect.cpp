#include "draw_rect.hpp"

/*
 * `SDL_Render` uses the coordinate system where the (0,0) is in the top left
 * corner and the positive Y value is in down direction, whereas the
 * TextRenderer uses the Y=0 at the bottom and positive Y value is in up
 * direction. The Draw functions here will convert the direction and the
 * position, so it's transparent to TextRendering function.
 *
 * The reason that Text Rendering function use positive Y value representing up
 * direction is to match the modern rendering apis such as OpenGL or DirectX.
 */

void DrawRect(Context &ctx, const float &x, const float &y, const float &w,
              const float &h, const SDL_Color &color,
              const DrawRectMode &mode) {

  if (w == 0 || h == 0)
    return;

  SDL_FRect rect{x, y, w, h};

  /*
   * Adjust the coordinate, and recalculate the new y origin of the rectangle.
   *
   * The given rectangle value has its origin in the bottom-left corner while
   * SDL expects the origin in the top-left corner.
   */
  rect.y = static_cast<float>(ctx.windowBound.h) - rect.y - rect.h;

  SDL_SetRenderDrawColor(ctx.renderer, color.r, color.g, color.b, color.a);
  SDL_SetRenderDrawBlendMode(ctx.renderer, SDL_BLENDMODE_BLEND);

  switch (mode) {

  case DrawRectMode::Fill:
    SDL_RenderFillRectF(ctx.renderer, &rect);
    break;

  case DrawRectMode::Outline:
    SDL_RenderDrawRectF(ctx.renderer, &rect);
    break;
  }
}

void DrawLine(Context &ctx, const float &x1, const float &y1, const float &x2,
              const float &y2, const SDL_Color &color) {
  SDL_SetRenderDrawColor(ctx.renderer, color.r, color.g, color.b, color.a);
  SDL_SetRenderDrawBlendMode(ctx.renderer, SDL_BLENDMODE_BLEND);

  float actualY1 = static_cast<float>(ctx.windowBound.h) - y1;
  float actualY2 = static_cast<float>(ctx.windowBound.h) - y2;

  SDL_RenderDrawLineF(ctx.renderer, x1, actualY1, x2, actualY2);
}