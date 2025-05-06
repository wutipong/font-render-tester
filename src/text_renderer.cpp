#include "text_renderer.hpp"

#include <utf8cpp/utf8.h>

#include "colors.hpp"
#include "draw_glyph.hpp"
#include "font.hpp"

namespace {
void DrawRect(SDL_Renderer *renderer, DebugSettings &debug, const float &x,
              const float &y, const float &w, const float &h,
              const SDL_Color &color) {

  if (w == 0 || h == 0)
    return;

  SDL_FRect rect{x, y, w, h};
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

  SDL_RenderFillRect(renderer, &rect);
}

void DrawLine(SDL_Renderer *renderer, DebugSettings &debug, const float &x1,
              const float &y1, const float &x2, const float &y2,
              const SDL_Color &color) {
  SDL_Rect bound;
  SDL_GetRenderViewport(renderer, &bound);

  SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

  float actualY1 = static_cast<float>(bound.h) - y1;
  float actualY2 = static_cast<float>(bound.h) - y2;

  SDL_RenderLine(renderer, x1, actualY1, x2, actualY2);
}

void DrawHorizontalLineDebug(SDL_Renderer *renderer, DebugSettings &debug,
                             const float &lineHeight, const float &ascend,
                             const float &descend) {
  if (!debug.enabled)
    return;

  SDL_Rect bound;
  SDL_GetRenderViewport(renderer, &bound);

  float y = bound.h - lineHeight;
  do {
    if (debug.debugAscend) {
      DrawRect(renderer, debug, 0, y, bound.w, ascend, debugAscendColor);
    }
    if (debug.debugDescend) {
      DrawRect(renderer, debug, 0, y, bound.w, descend, debugDescendColor);
    }
    if (debug.debugBaseline) {
      DrawLine(renderer, debug, 0, y, bound.w, y, debugBaselineColor);
    }
    y -= lineHeight;
  } while (y > 0);
}

void DrawVerticalLineDebug(SDL_Renderer *renderer, DebugSettings &debug,
                           const float &lineWidth, const float &ascend,
                           const float &descend) {
  if (!debug.enabled)
    return;

  SDL_Rect bound;
  SDL_GetRenderViewport(renderer, &bound);

  float x = bound.w - lineWidth;
  do {
    if (debug.debugAscend) {
      DrawRect(renderer, debug, x, 0, ascend, bound.h, debugAscendColor);
    }
    if (debug.debugDescend) {
      DrawRect(renderer, debug, x, 0, descend, bound.h, debugDescendColor);
    }
    if (debug.debugBaseline) {
      DrawLine(renderer, debug, x, 0, x, bound.h, debugBaselineColor);
    }
    x += lineWidth;
  } while (x > 0);
}
} // namespace

void TextRenderNoShape(SDL_Renderer *renderer, DebugSettings &debug, Font &font,
                       const std::string &str, const SDL_Color &color) {
  if (!font.IsValid())
    return;

  SDL_Rect bound;
  SDL_GetRenderViewport(renderer, &bound);

  int x = 0, y = bound.h - font.LineHeight();
  auto u16str = utf8::utf8to16(str);

  DrawHorizontalLineDebug(renderer, debug, font.LineHeight(), font.Ascend(),
                          font.Descend());

  for (auto &u : u16str) {
    if (u == '\n') {
      x = 0;
      y -= font.LineHeight();

      continue;
    }

    auto &g = font.GetGlyphFromChar(renderer, u);
    DrawGlyph(renderer, debug, font, g, color, x, y);
    x += g.advance;
  }
}

void TextRenderLeftToRight(SDL_Renderer *renderer, DebugSettings &debug, Font &font,
                           const std::string &str, const SDL_Color &color,
                           const std::string &language,
                           const hb_script_t &script) {
  if (!font.IsValid())
    return;

  SDL_Rect bound;
  SDL_GetRenderViewport(renderer, &bound);

  auto u16str = utf8::utf8to16(str);
  auto lineStart = u16str.begin();
  int y = bound.h - font.LineHeight();

  DrawHorizontalLineDebug(renderer, debug, font.LineHeight(), font.Ascend(),
                          font.Descend());

  while (true) {
    auto lineEnd = std::find(lineStart, u16str.end(), '\n');

    std::u16string line(lineStart, lineEnd);

    hb_buffer_t *buffer = hb_buffer_create();
    hb_buffer_set_direction(buffer, HB_DIRECTION_LTR);

    if (!language.empty())
      hb_buffer_set_language(
          buffer, hb_language_from_string(language.c_str(), language.length()));

    hb_buffer_set_script(buffer, script);

    hb_buffer_add_utf16(buffer,
                        reinterpret_cast<const uint16_t *>(line.c_str()),
                        line.size(), 0, line.size());

    hb_shape(font.HbFont(), buffer, NULL, 0);

    unsigned int glyph_count = hb_buffer_get_length(buffer);
    hb_glyph_info_t *glyph_infos = hb_buffer_get_glyph_infos(buffer, NULL);
    hb_glyph_position_t *glyph_positions =
        hb_buffer_get_glyph_positions(buffer, NULL);

    int x = 0;

    for (int i = 0; i < glyph_count; i++) {
      auto index = glyph_infos[i].codepoint;

      auto &g = font.GetGlyph(renderer, index);
      DrawGlyph(renderer, debug, font, g, color, x, y, glyph_positions[i]);
      x += g.advance;
    }
    hb_buffer_destroy(buffer);

    if (lineEnd == u16str.end())
      break;

    lineStart = lineEnd + 1;
    y -= font.LineHeight();
  }
}

void TextRenderRightToLeft(SDL_Renderer *renderer, DebugSettings &debug, Font &font,
                           const std::string &str, const SDL_Color &color,
                           const std::string &language,
                           const hb_script_t &script) {
  if (!font.IsValid())
    return;

  auto u16str = utf8::utf8to16(str);
  auto lineStart = u16str.begin();

  hb_font_extents_t extents;
  hb_font_get_extents_for_direction(font.HbFont(), HB_DIRECTION_RTL, &extents);

  SDL_Rect bound;
  SDL_GetRenderViewport(renderer, &bound);

  int y = bound.h - font.LineHeight();

  DrawHorizontalLineDebug(renderer, debug, font.LineHeight(), font.Ascend(),
                          font.Descend());

  while (true) {
    auto lineEnd = std::find(lineStart, u16str.end(), '\n');

    std::u16string line(lineStart, lineEnd);

    hb_buffer_t *buffer = hb_buffer_create();
    hb_buffer_set_direction(buffer, HB_DIRECTION_RTL);

    if (!language.empty())
      hb_buffer_set_language(
          buffer, hb_language_from_string(language.c_str(), language.length()));

    hb_buffer_set_script(buffer, script);

    hb_buffer_add_utf16(buffer,
                        reinterpret_cast<const uint16_t *>(line.c_str()),
                        line.size(), 0, line.size());

    hb_shape(font.HbFont(), buffer, NULL, 0);

    unsigned int glyph_count = hb_buffer_get_length(buffer);
    hb_glyph_info_t *glyph_infos = hb_buffer_get_glyph_infos(buffer, NULL);
    hb_glyph_position_t *glyph_positions =
        hb_buffer_get_glyph_positions(buffer, NULL);

    int x = bound.w;

    for (int i = glyph_count - 1; i >= 0; i--) {
      auto index = glyph_infos[i].codepoint;

      auto &g = font.GetGlyph(renderer, index);
      x -= HBPosToFloat(glyph_positions[i].x_advance);
      DrawGlyph(renderer, debug, font, g, color, x, y, glyph_positions[i]);
    }
    hb_buffer_destroy(buffer);

    if (lineEnd == u16str.end())
      break;

    lineStart = lineEnd + 1;
    y -= font.LineHeight();
  }
}

void TextRenderTopToBottom(SDL_Renderer *renderer, DebugSettings &debug, Font &font,
                           const std::string &str, const SDL_Color &color,
                           const std::string &language,
                           const hb_script_t &script) {
  if (!font.IsValid())
    return;
    
  SDL_Rect bound;
  SDL_GetRenderViewport(renderer, & bound);

  hb_font_extents_t extents;
  hb_font_get_extents_for_direction(font.HbFont(), HB_DIRECTION_TTB, &extents);

  const float ascend = HBPosToFloat(extents.ascender);
  const float descend = HBPosToFloat(extents.descender);
  const float linegap = HBPosToFloat(extents.line_gap);

  const auto lineWidth = -ascend + descend + linegap;

  auto u16str = utf8::utf8to16(str);
  auto lineStart = u16str.begin();
  int x = bound.w + lineWidth;

  if (debug.enabled) {
    DrawVerticalLineDebug(renderer, debug, lineWidth, ascend, descend);
  }

  while (true) {
    auto lineEnd = std::find(lineStart, u16str.end(), '\n');

    std::u16string line(lineStart, lineEnd);

    hb_buffer_t *buffer = hb_buffer_create();
    hb_buffer_set_direction(buffer, HB_DIRECTION_TTB);

    if (!language.empty())
      hb_buffer_set_language(
          buffer, hb_language_from_string(language.c_str(), language.length()));

    hb_buffer_set_script(buffer, script);

    hb_buffer_add_utf16(buffer,
                        reinterpret_cast<const uint16_t *>(line.c_str()),
                        line.size(), 0, line.size());

    hb_shape(font.HbFont(), buffer, NULL, 0);

    unsigned int glyph_count = hb_buffer_get_length(buffer);
    hb_glyph_info_t *glyph_infos = hb_buffer_get_glyph_infos(buffer, NULL);
    hb_glyph_position_t *glyph_positions =
        hb_buffer_get_glyph_positions(buffer, NULL);

    int y = bound.h;

    for (int i = 0; i < glyph_count; i++) {
      auto index = glyph_infos[i].codepoint;

      auto &g = font.GetGlyph(renderer, index);
      DrawGlyph(renderer, debug, font, g, color, x, y, glyph_positions[i]);

      y += HBPosToFloat(glyph_positions[i].y_advance);
    }
    hb_buffer_destroy(buffer);

    if (lineEnd == u16str.end())
      break;

    lineStart = lineEnd + 1;
    x += lineWidth;
  }
}