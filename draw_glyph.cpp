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

void DrawGlyph(Context &ctx, const Font &font, const Glyph &g,
               const SDL_Color &color, const int &x, const int &y) {
  // DrawGlyph(renderer, g, x, y, color, ctx.windowBound.w, ctx.windowBound.h);

  SDL_FRect rect{static_cast<float>(x + g.bound.x),
                 static_cast<float>(y + g.bound.y),
                 static_cast<float>(g.bound.w), static_cast<float>(g.bound.h)};

  /*
   * Adjust the coordinate, and recalculate the new y origin of the rectangle.
   *
   * The given rectangle value has its origin in the bottom-left corner while
   * SDL expects the origin in the top-left corner.
   */
  rect.y = static_cast<float>(ctx.windowBound.h) - rect.y - rect.h;

  SDL_SetRenderDrawColor(ctx.renderer, color.r, color.g, color.b, color.a);
  SDL_SetRenderDrawBlendMode(ctx.renderer, SDL_BLENDMODE_BLEND);
  SDL_SetTextureColorMod(g.texture, color.r, color.g, color.b);

  SDL_RenderCopyF(ctx.renderer, g.texture, nullptr, &rect);

  if (ctx.debug) {
    DrawRect(ctx, x + g.bound.x, y + g.bound.y, g.bound.w, g.bound.h,
             ctx.debugGlyphBoundColor);
  }
}

void DrawGlyph(Context &ctx, const Font &font, const Glyph &g,
               const SDL_Color &color, const int &x, const int &y,
               const hb_glyph_position_t &hb_glyph_pos) {

  // when using with hb-ft, the position metrics will be 26.6 format as well as
  // the face->metrics.
  auto xPos = x + (hb_glyph_pos.x_offset / 64.0f);
  auto yPos = y + (hb_glyph_pos.y_offset / 64.0f);

  DrawGlyph(ctx, font, g, color, xPos, yPos);
}
