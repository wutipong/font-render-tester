#include "draw_glyph.hpp"

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

void DrawGlyph(SDL_Renderer *renderer, const Glyph &glyph, const float &x, const float &y,
               const SDL_Color &color, const int &screenWidth,
               const int &screenHeight) {

  SDL_FRect rect {x + glyph.bound.x, y + glyph.bound.y, static_cast<float>(glyph.bound.w), static_cast<float>(glyph.bound.h)};

  /*
   * Adjust the coordinate, and recalculate the new y origin of the rectangle.
   *
   * The given rectangle value has its origin in the bottom-left corner while
   * SDL expects the origin in the top-left corner.
   */
  rect.y = static_cast<float>(screenHeight) - rect.y - rect.h;

  SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
  SDL_SetTextureColorMod(glyph.texture, color.r, color.g, color.b);

  SDL_RenderCopyF(renderer, glyph.texture, nullptr, &rect);
}
