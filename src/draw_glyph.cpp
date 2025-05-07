#include "draw_glyph.hpp"

#include "colors.hpp"
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

void DrawGlyph(SDL_Renderer *renderer, DebugSettings &debug, const Font &font,
               const Glyph &g, const SDL_Color &color, const int &x,
               const int &y) {

  SDL_FRect rect{
      static_cast<float>(x + g.bound.x),
      static_cast<float>(y + g.bound.y),
      static_cast<float>(g.bound.w),
      static_cast<float>(g.bound.h),
  };

  SDL_Rect bound;
  SDL_GetRenderViewport(renderer, &bound);

  /*
   * Adjust the coordinate, and recalculate the new y origin of the rectangle.
   *
   * The given rectangle value has its origin in the bottom-left corner while
   * SDL expects the origin in the top-left corner.
   */
  rect.y = static_cast<float>(bound.h) - rect.y - rect.h;

  SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
  SDL_SetTextureColorMod(g.texture, color.r, color.g, color.b);

  SDL_RenderTexture(renderer, g.texture, nullptr, &rect);

  if (debug.enabled && debug.debugGlyphBound) {
    SDL_SetRenderDrawColor(renderer, debugGlyphBoundColor.r,
                           debugGlyphBoundColor.g, debugGlyphBoundColor.b,
                           debugGlyphBoundColor.a);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_RenderRect(renderer, &rect);
  }

  if (debug.enabled && debug.debugCaret) {
    SDL_FRect rect{
        static_cast<float>(x),
        static_cast<float>(y),
        1,
        1,
    };

    rect.y = static_cast<float>(bound.h) - rect.y - rect.h;

    SDL_SetRenderDrawColor(renderer, debugCaretColor.r, debugCaretColor.g,
                           debugCaretColor.b, debugCaretColor.a);

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_RenderRect(renderer, &rect);
  }
}

void DrawGlyph(SDL_Renderer *renderer, DebugSettings &debug, const Font &font,
               const Glyph &g, const SDL_Color &color, const int &x,
               const int &y, const hb_glyph_position_t &hb_glyph_pos) {

  auto xPos = x + HBPosToFloat(hb_glyph_pos.x_offset);
  auto yPos = y + HBPosToFloat(hb_glyph_pos.y_offset);

  DrawGlyph(renderer, debug, font, g, color, xPos, yPos);
}
