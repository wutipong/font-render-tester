#include "text_renderer.hpp"

#include <utf8/cpp11.h>

#include "font.hpp"

void TextRenderNoShape(SDL_Renderer *renderer, const SDL_Rect &bound,
                       Font &font, const std::string &str,
                       const SDL_Color &color, const hb_script_t &script) {

  if (!font.IsValid())
    return;

  int x = 0, y = font.Ascend();
  auto u16str = utf8::utf8to16(str);

  for (auto &u : u16str) {
    if (u == '\n') {
      x = 0;
      y += -font.Descend() + font.LineGap() + font.Ascend();

      continue;
    }

    auto &g = font.GetGlyphFromChar(renderer, u);
    DrawGlyph(renderer, font, g, color, x, y);
    x += g.advance;
  }
}

void TextRenderLeftToRight(SDL_Renderer *renderer, const SDL_Rect &bound,
                           Font &font, const std::string &str,
                           const SDL_Color &color, const hb_script_t &script) {
  if (!font.IsValid())
    return;

  auto u16str = utf8::utf8to16(str);
  auto lineStart = u16str.begin();
  int y = font.Ascend();

  while (true) {
    auto lineEnd = std::find(lineStart, u16str.end(), '\n');

    std::u16string line(lineStart, lineEnd);

    hb_buffer_t *buffer = hb_buffer_create();
    hb_buffer_set_direction(buffer, HB_DIRECTION_LTR);
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
      DrawGlyph(renderer, font, g, color, x, y, glyph_positions[i]);
      x += g.advance;
    }
    hb_buffer_destroy(buffer);

    if (lineEnd == u16str.end())
      break;

    lineStart = lineEnd + 1;
    y += -font.Descend() + font.LineGap() + font.Ascend();
  }
}

void TextRenderRightToLeft(SDL_Renderer *renderer, const SDL_Rect &bound,
                           Font &font, const std::string &str,
                           const SDL_Color &color, const hb_script_t &script) {
  if (!font.IsValid())
    return;

  auto u16str = utf8::utf8to16(str);
  auto lineStart = u16str.begin();

  hb_font_extents_t extents;
  hb_font_get_extents_for_direction(font.HbFont(), HB_DIRECTION_RTL, &extents);

  int y = roundf(extents.ascender * font.Scale());

  while (true) {
    auto lineEnd = std::find(lineStart, u16str.end(), '\n');

    std::u16string line(lineStart, lineEnd);

    hb_buffer_t *buffer = hb_buffer_create();
    hb_buffer_set_direction(buffer, HB_DIRECTION_LTR);
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

    for (int i = 0; i < glyph_count; i++) {
      auto index = glyph_infos[i].codepoint;

      auto &g = font.GetGlyph(renderer, index);
      DrawGlyph(renderer, font, g, color, x, y, glyph_positions[i]);
      x -= glyph_positions[i].x_advance * font.Scale();
    }
    hb_buffer_destroy(buffer);

    if (lineEnd == u16str.end())
      break;

    lineStart = lineEnd + 1;
    y += -font.Descend() + font.LineGap() + font.Ascend();
  }
}

void TextRenderTopToBottom(SDL_Renderer *renderer, const SDL_Rect &bound,
                           Font &font, const std::string &str,
                           const SDL_Color &color, const hb_script_t &script) {

  if (!font.IsValid())
    return;

  hb_font_extents_t extents;
  hb_font_get_extents_for_direction(font.HbFont(), HB_DIRECTION_TTB, &extents);

  float ascend = roundf(extents.ascender * font.Scale());
  float descend = roundf(extents.descender * font.Scale());
  float linegap = roundf(extents.descender * font.Scale());

  auto u16str = utf8::utf8to16(str);
  auto lineStart = u16str.begin();
  int x = bound.w - ascend;

  while (true) {

    SDL_SetRenderDrawColor(renderer, 0xff, 0, 0, 0xff);

    auto lineEnd = std::find(lineStart, u16str.end(), '\n');

    std::u16string line(lineStart, lineEnd);

    hb_buffer_t *buffer = hb_buffer_create();
    hb_buffer_set_direction(buffer, HB_DIRECTION_TTB);
    hb_buffer_set_script(buffer, script);

    hb_buffer_add_utf16(buffer,
                        reinterpret_cast<const uint16_t *>(line.c_str()),
                        line.size(), 0, line.size());

    hb_shape(font.HbFont(), buffer, NULL, 0);

    unsigned int glyph_count = hb_buffer_get_length(buffer);
    hb_glyph_info_t *glyph_infos = hb_buffer_get_glyph_infos(buffer, NULL);
    hb_glyph_position_t *glyph_positions =
        hb_buffer_get_glyph_positions(buffer, NULL);

    int y = 0;

    for (int i = 0; i < glyph_count; i++) {
      auto index = glyph_infos[i].codepoint;

      auto &g = font.GetGlyph(renderer, index);
      DrawGlyph(renderer, font, g, color, x, y, glyph_positions[i]);

      y -= roundf(glyph_positions[i].y_advance * font.Scale());
    }
    hb_buffer_destroy(buffer);

    if (lineEnd == u16str.end())
      break;

    lineStart = lineEnd + 1;
    x -= -font.Descend() + font.LineGap() + font.Ascend();
  }
}

void DrawGlyph(SDL_Renderer *renderer, const Font& font, const Glyph &g, const SDL_Color &color,
               const int &x, const int &y) {
  if (g.texture != nullptr) {
    SDL_SetTextureColorMod(g.texture, color.r, color.g, color.b);

    SDL_Rect dst = g.bound;
    dst.x += x;
    dst.y = y + dst.y;

    SDL_RenderCopy(renderer, g.texture, nullptr, &dst);
  }
}

void DrawGlyph(SDL_Renderer *renderer, const Font& font, const Glyph &g, const SDL_Color &color,
               const int &x, const int &y,
               const hb_glyph_position_t &hb_glyph_pos) {
  if (g.texture != nullptr) {
    SDL_SetTextureColorMod(g.texture, color.r, color.g, color.b);

    SDL_Rect dst = g.bound;
    dst.x += x + (hb_glyph_pos.x_offset * font.Scale());
    dst.y = y + dst.y - (hb_glyph_pos.y_offset * font.Scale());

    SDL_RenderCopy(renderer, g.texture, nullptr, &dst);
  }
}
