#include "text_renderer.hpp"

#include <utf8cpp/utf8.h>

#include "draw_glyph.hpp"
#include "draw_rect.hpp"
#include "font.hpp"

void InitTextRenderers() {
  InitDrawGlyph();
}

void CleanUpTextRenderers() {
  CleanUpDrawGlyph();
}

void DrawLineDebug(SDL_Renderer* renderer, Context &ctx, Font &font) {
  float y = ctx.windowBound.h - font.LineHeight();
  do {
    DrawRect(renderer, 0, y, ctx.windowBound.w, font.Ascend(), ctx.debugAscendColor,
             ctx.windowBound.w, ctx.windowBound.h, DrawRectMode::Fill);

    DrawRect(renderer, 0, y + font.Descend(), ctx.windowBound.w, -font.Descend(),
             ctx.debugDescendColor, ctx.windowBound.w, ctx.windowBound.h,
             DrawRectMode::Fill);

    DrawLine(renderer, 0, y, ctx.windowBound.w, y, ctx.debugLineColor, ctx.windowBound.w,
             ctx.windowBound.h);
    y -= font.LineHeight();
  } while (y > 0);
}

void TextRenderNoShape(SDL_Renderer *renderer, Context &ctx, Font &font,
                       const std::string &str, const SDL_Color &color,
                       const std::string &language, const hb_script_t &script) {
  if (!font.IsValid())
    return;

  int x = 0, y = ctx.windowBound.h - font.LineHeight();
  auto u16str = utf8::utf8to16(str);

  if (ctx.debug) {
    DrawLineDebug(renderer, ctx, font);
  }

  for (auto &u : u16str) {
    if (u == '\n') {
      x = 0;
      y -= font.LineHeight();

      continue;
    }

    auto &g = font.GetGlyphFromChar(u, renderer);
    DrawGlyph(renderer, ctx, font, g, color, x, y);
    x += g.advance;
  }
}

void TextRenderLeftToRight(SDL_Renderer *renderer, Context &ctx, Font &font,
                           const std::string &str, const SDL_Color &color,
                           const std::string &language,
                           const hb_script_t &script) {
  if (!font.IsValid())
    return;

  auto u16str = utf8::utf8to16(str);
  auto lineStart = u16str.begin();
  int y = ctx.windowBound.h - font.LineHeight();

  if (ctx.debug) {
    DrawLineDebug(renderer, ctx, font);
  }
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

      auto &g = font.GetGlyph(index, renderer);
      DrawGlyph(renderer, ctx, font, g, color, x, y, glyph_positions[i]);
      x += g.advance;
    }
    hb_buffer_destroy(buffer);

    if (lineEnd == u16str.end())
      break;

    lineStart = lineEnd + 1;
    y -= font.LineHeight();
  }
}

void TextRenderRightToLeft(SDL_Renderer *renderer, Context &ctx, Font &font,
                           const std::string &str, const SDL_Color &color,
                           const std::string &language,
                           const hb_script_t &script) {
  if (!font.IsValid())
    return;

  auto u16str = utf8::utf8to16(str);
  auto lineStart = u16str.begin();

  hb_font_extents_t extents;
  hb_font_get_extents_for_direction(font.HbFont(), HB_DIRECTION_RTL, &extents);

  int y = ctx.windowBound.h - roundf(extents.ascender / 64.0f);

  while (true) {
    if (ctx.debug) {
      DrawRect(renderer, 0, y, ctx.windowBound.w, 0, ctx.debugLineColor,
               ctx.windowBound.w, ctx.windowBound.h);
    }
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
    const auto lineHeight = -font.Descend() + font.LineGap() + font.Ascend();

    for (int i = glyph_count - 1; i >= 0; i--) {
      auto index = glyph_infos[i].codepoint;

      auto &g = font.GetGlyph(index, renderer);
      x -= glyph_positions[i].x_advance / 64.0f;
      DrawGlyph(renderer, ctx, font, g, color, x, y, glyph_positions[i]);
    }
    hb_buffer_destroy(buffer);

    if (lineEnd == u16str.end())
      break;

    lineStart = lineEnd + 1;
    y -= lineHeight;
  }
}

void TextRenderTopToBottom(SDL_Renderer *renderer, Context &ctx, Font &font,
                           const std::string &str, const SDL_Color &color,
                           const std::string &language,
                           const hb_script_t &script) {
  if (!font.IsValid())
    return;

  hb_font_extents_t extents;
  hb_font_get_extents_for_direction(font.HbFont(), HB_DIRECTION_TTB, &extents);

  // when using with hb-ft, the position metrics will be 26.6 format as well as
  // the face->metrics.
  float ascend = roundf(extents.ascender / 64.0f);
  float descend = roundf(extents.descender / 64.0f);
  float linegap = roundf(extents.line_gap / 64.0f);

  auto u16str = utf8::utf8to16(str);
  auto lineStart = u16str.begin();
  int x = ctx.windowBound.w - ascend;

  const auto lineWidth = -ascend + descend + linegap;

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

      auto &g = font.GetGlyph(index, renderer);
      DrawGlyph(renderer, ctx, font, g, color, x, y, glyph_positions[i]);

      y += roundf(glyph_positions[i].y_advance / 64.0f);
    }
    hb_buffer_destroy(buffer);

    if (ctx.debug) {
      DrawRect(renderer, x, 0, x, ctx.windowBound.h, ctx.debugLineColor,
               ctx.windowBound.w, ctx.windowBound.h);
    }

    if (lineEnd == u16str.end())
      break;

    lineStart = lineEnd + 1;
    x += lineWidth;
  }
}

void DrawGlyph(SDL_Renderer *renderer, Context &ctx, const Font &font, const Glyph &g,
               const SDL_Color &color, const int &x, const int &y) {
  DrawGlyph(g, x, y, color, ctx.windowBound.w, ctx.windowBound.h);

  if (ctx.debug) {
    DrawRect(renderer, x + g.bound.x, y + g.bound.y, g.bound.w, g.bound.h,
             ctx.debugGlyphBoundColor, ctx.windowBound.w, ctx.windowBound.h);
  }
}

void DrawGlyph(SDL_Renderer *renderer,Context &ctx, const Font &font, const Glyph &g,
               const SDL_Color &color, const int &x, const int &y,
               const hb_glyph_position_t &hb_glyph_pos) {

  // when using with hb-ft, the position metrics will be 26.6 format as well as
  // the face->metrics.
  auto xPos = x + (hb_glyph_pos.x_offset / 64.0f);
  auto yPos = y + (hb_glyph_pos.y_offset / 64.0f);

  DrawGlyph(g, xPos, yPos, color, ctx.windowBound.w, ctx.windowBound.h);

  if (ctx.debug) {
    DrawRect(renderer, xPos + g.bound.x, yPos + g.bound.y, g.bound.w, g.bound.h,
             ctx.debugGlyphBoundColor, ctx.windowBound.w, ctx.windowBound.h);
  }
}
