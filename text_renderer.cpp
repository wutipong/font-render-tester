#include "text_renderer.hpp"

#include <utf8cpp/utf8.h>

#include "draw_glyph.hpp"
#include "font.hpp"

namespace {
void DrawRect(Context &ctx, const float &x, const float &y, const float &w,
              const float &h, const SDL_Color &color) {

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

  SDL_RenderFillRectF(ctx.renderer, &rect);
}

void DrawLine(Context &ctx, const float &x1, const float &y1, const float &x2,
              const float &y2, const SDL_Color &color) {
  SDL_SetRenderDrawColor(ctx.renderer, color.r, color.g, color.b, color.a);
  SDL_SetRenderDrawBlendMode(ctx.renderer, SDL_BLENDMODE_BLEND);

  float actualY1 = static_cast<float>(ctx.windowBound.h) - y1;
  float actualY2 = static_cast<float>(ctx.windowBound.h) - y2;

  SDL_RenderDrawLineF(ctx.renderer, x1, actualY1, x2, actualY2);
}

void DrawHorizontalLineDebug(Context &ctx, const float &lineHeight,
                             const float &ascend, const float &descend) {
  if (!ctx.debug)
    return;

  float y = ctx.windowBound.h - lineHeight;
  do {
    if (ctx.debugAscend) {
      DrawRect(ctx, 0, y, ctx.windowBound.w, ascend, ctx.debugAscendColor);
    }
    if (ctx.debugDescend) {
      DrawRect(ctx, 0, y, ctx.windowBound.w, descend, ctx.debugDescendColor);
    }
    if (ctx.debugBaseline) {
      DrawLine(ctx, 0, y, ctx.windowBound.w, y, ctx.debugBaselineColor);
    }
    y -= lineHeight;
  } while (y > 0);
}

void DrawVerticalLineDebug(Context &ctx, const float &lineWidth,
                           const float &ascend, const float &descend) {
  if (!ctx.debug)
    return;

  float x = ctx.windowBound.w - lineWidth;
  do {
    if (ctx.debugAscend) {
      DrawRect(ctx, x, 0, ascend, ctx.windowBound.h, ctx.debugAscendColor);
    }
    if (ctx.debugDescend) {
      DrawRect(ctx, x, 0, descend, ctx.windowBound.h, ctx.debugDescendColor);
    }
    if (ctx.debugBaseline) {
      DrawLine(ctx, x, 0, x, ctx.windowBound.h, ctx.debugBaselineColor);
    }
    x += lineWidth;
  } while (x > 0);
}
} // namespace

void TextRenderNoShape(Context &ctx, Font &font, const std::string &str,
                       const SDL_Color &color, const std::string &language,
                       const hb_script_t &script) {
  if (!font.IsValid())
    return;

  int x = 0, y = ctx.windowBound.h - font.LineHeight();
  auto u16str = utf8::utf8to16(str);

  DrawHorizontalLineDebug(ctx, font.LineHeight(), font.Ascend(),
                          font.Descend());

  for (auto &u : u16str) {
    if (u == '\n') {
      x = 0;
      y -= font.LineHeight();

      continue;
    }

    auto &g = font.GetGlyphFromChar(ctx, u);
    DrawGlyph(ctx, font, g, color, x, y);
    x += g.advance;
  }
}

void TextRenderLeftToRight(Context &ctx, Font &font, const std::string &str,
                           const SDL_Color &color, const std::string &language,
                           const hb_script_t &script) {
  if (!font.IsValid())
    return;

  auto u16str = utf8::utf8to16(str);
  auto lineStart = u16str.begin();
  int y = ctx.windowBound.h - font.LineHeight();

  DrawHorizontalLineDebug(ctx, font.LineHeight(), font.Ascend(),
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

      auto &g = font.GetGlyph(ctx, index);
      DrawGlyph(ctx, font, g, color, x, y, glyph_positions[i]);
      x += g.advance;
    }
    hb_buffer_destroy(buffer);

    if (lineEnd == u16str.end())
      break;

    lineStart = lineEnd + 1;
    y -= font.LineHeight();
  }
}

void TextRenderRightToLeft(Context &ctx, Font &font, const std::string &str,
                           const SDL_Color &color, const std::string &language,
                           const hb_script_t &script) {
  if (!font.IsValid())
    return;

  auto u16str = utf8::utf8to16(str);
  auto lineStart = u16str.begin();

  hb_font_extents_t extents;
  hb_font_get_extents_for_direction(font.HbFont(), HB_DIRECTION_RTL, &extents);

  int y = ctx.windowBound.h - font.LineHeight();

  DrawHorizontalLineDebug(ctx, font.LineHeight(), font.Ascend(),
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

    int x = ctx.windowBound.w;

    for (int i = glyph_count - 1; i >= 0; i--) {
      auto index = glyph_infos[i].codepoint;

      auto &g = font.GetGlyph(ctx, index);
      x -= HBPosToFloat(glyph_positions[i].x_advance);
      DrawGlyph(ctx, font, g, color, x, y, glyph_positions[i]);
    }
    hb_buffer_destroy(buffer);

    if (lineEnd == u16str.end())
      break;

    lineStart = lineEnd + 1;
    y -= font.LineHeight();
  }
}

void TextRenderTopToBottom(Context &ctx, Font &font, const std::string &str,
                           const SDL_Color &color, const std::string &language,
                           const hb_script_t &script) {
  if (!font.IsValid())
    return;

  hb_font_extents_t extents;
  hb_font_get_extents_for_direction(font.HbFont(), HB_DIRECTION_TTB, &extents);

  const float ascend = HBPosToFloat(extents.ascender);
  const float descend = HBPosToFloat(extents.descender);
  const float linegap = HBPosToFloat(extents.line_gap);

  const auto lineWidth = -ascend + descend + linegap;

  auto u16str = utf8::utf8to16(str);
  auto lineStart = u16str.begin();
  int x = ctx.windowBound.w + lineWidth;

  if (ctx.debug) {
    DrawVerticalLineDebug(ctx, lineWidth, ascend, descend);
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

    int y = ctx.windowBound.h;

    for (int i = 0; i < glyph_count; i++) {
      auto index = glyph_infos[i].codepoint;

      auto &g = font.GetGlyph(ctx, index);
      DrawGlyph(ctx, font, g, color, x, y, glyph_positions[i]);

      y += HBPosToFloat(glyph_positions[i].y_advance);
    }
    hb_buffer_destroy(buffer);

    if (lineEnd == u16str.end())
      break;

    lineStart = lineEnd + 1;
    x += lineWidth;
  }
}