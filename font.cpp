#include "font.hpp"
#include <fstream>
#include <streambuf>

#include <utf8/cpp11.h>

#include "texture.hpp"

Font::Font(){};

Font::Font(const std::string &path) {
  Font::LoadFile(path, data);
  Initialize();
}

Font::Font(const Font &f) : Font(f.data) {}

Font::Font(const std::vector<unsigned char> &data)
    : data(data.begin(), data.end()) {
  Initialize();
}

Font &Font::operator=(const Font &f) {
  data = f.data;
  Initialize();

  return *this;
}

Font::~Font() {
  hb_font_destroy(hb_font);
  hb_font = nullptr;

  for (auto &g : glyphMap) {
    SDL_DestroyTexture(g.second.texture);
  }

  data.clear();
}

void Font::Initialize() {
  stbtt_InitFont(&font, reinterpret_cast<unsigned char *>(data.data()), 0);

  hb_blob_t *blob =
      hb_blob_create(reinterpret_cast<char *>(data.data()), data.size(),
                     HB_MEMORY_MODE_READONLY, nullptr, nullptr);
  hb_face_t *hb_face = hb_face_create(blob, 0);
  hb_blob_destroy(blob);

  hb_font = hb_font_create(hb_face);

  stbtt_GetFontVMetrics(&font, &ascend, &descend, &linegap);
  Invalidate();
  fontSize = -1;
}

void Font::Invalidate() { glyphMap.clear(); }

void Font::DrawTextWithoutShaping(SDL_Renderer *renderer,
                                  const std::string &str,
                                  const SDL_Color &color) {
  if (data.empty())
    return;

  int x = 0, y = scaledAscend;
  auto u16str = utf8::utf8to16(str);

  for (auto &u : u16str) {
    if (u == '\n') {
      x = 0;
      y += -scaledDescend + scaledLinegap + scaledAscend;

      continue;
    }

    auto iter = glyphMap.find(u);
    if (iter == glyphMap.end()) {
      Glyph g = CreateGlyphFromChar(renderer, u);
      auto [i, success] = glyphMap.insert({u, g});

      iter = i;
    }

    auto &g = iter->second;
    if (g.texture != nullptr) {
      SDL_SetTextureColorMod(g.texture, color.r, color.g, color.b);

      SDL_Rect dst = g.bound;
      dst.x += x;
      dst.y = y + dst.y;

      SDL_RenderCopy(renderer, g.texture, nullptr, &dst);
    }
    x += g.advance;
  }
}

void Font::DrawTextWithShaping(SDL_Renderer *renderer, const std::string &str,
                               const SDL_Color &color) {
  if (data.empty())
    return;

  auto u16str = utf8::utf8to16(str);
  auto lineStart = u16str.begin();
  int y = scaledAscend;

  while (true) {
    auto lineEnd = std::find(lineStart, u16str.end(), '\n');

    std::u16string line(lineStart, lineEnd);
    hb_buffer_t *buffer = hb_buffer_create();
    hb_buffer_set_direction(buffer, HB_DIRECTION_LTR);

    hb_buffer_add_utf16(buffer,
                        reinterpret_cast<const uint16_t *>(line.c_str()),
                        line.size(), 0, line.size());

    hb_shape(hb_font, buffer, NULL, 0);

    unsigned int glyph_count = hb_buffer_get_length(buffer);
    hb_glyph_info_t *glyph_infos = hb_buffer_get_glyph_infos(buffer, NULL);
    hb_glyph_position_t *glyph_positions =
        hb_buffer_get_glyph_positions(buffer, NULL);

    int x = 0;

    for (int i = 0; i < glyph_count; i++) {
      auto index = glyph_infos[i].codepoint;

      auto iter = glyphMap.find(index);
      if (iter == glyphMap.end()) {
        Glyph g = CreateGlyph(renderer, index);
        auto [it, success] = glyphMap.insert({index, g});

        iter = it;
      }

      auto &g = iter->second;
      if (g.texture != nullptr) {
        SDL_SetTextureColorMod(g.texture, color.r, color.g, color.b);

        SDL_Rect dst = g.bound;
        dst.x += x + (glyph_positions[i].x_offset *scale);
        dst.y = y + dst.y - (glyph_positions[i].y_offset *scale);

        SDL_RenderCopy(renderer, g.texture, nullptr, &dst);
      }
      x += g.advance;
    }
    hb_buffer_destroy(buffer);

    if (lineEnd == u16str.end())
      break;

    lineStart = lineEnd + 1;
    y += -scaledDescend + scaledLinegap + scaledAscend;
  }
}

void Font::SetFontSize(const int &size) {
  if (data.empty())
    return;

  if (fontSize == size) {
    return;
  }

  fontSize = size;
  Invalidate();

  scale = stbtt_ScaleForPixelHeight(&font, fontSize);
  scaledAscend = roundf(scale * ascend);
  scaledDescend = roundf(scale * descend);
  scaledLinegap = roundf(scale * linegap);
}

void Font::LoadFile(const std::string &path, std::vector<unsigned char> &data) {
  std::basic_ifstream<unsigned char> file(path, std::ios::binary);
  data =
      std::vector<unsigned char>{std::istreambuf_iterator<unsigned char>(file),
                                 std::istreambuf_iterator<unsigned char>()};
  file.close();
}

Font::Glyph Font::CreateGlyph(SDL_Renderer *renderer, const int &index) {
  int bearing;
  int advance;

  auto format = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA32);

  stbtt_GetGlyphHMetrics(&font, index, &advance, &bearing);

  advance = static_cast<int>(roundf(advance * scale));
  bearing = static_cast<int>(roundf(bearing * scale));

  int x1, y1, x2, y2;
  stbtt_GetGlyphBitmapBox(&font, index, scale, scale, &x1, &y1, &x2, &y2);

  int width = x2 - x1;
  int height = y2 - y1;

  auto bitmap = new unsigned char[width * height];

  stbtt_MakeGlyphBitmap(&font, bitmap, width, height, width, scale, scale,
                        index);

  auto texture =
      CreateTextureFromBitmap(renderer, format, bitmap, width, height);

  delete[] bitmap;

  SDL_Rect bound{x1, y1, width, height};

  SDL_FreeFormat(format);

  return {texture, bound, advance, bearing};
}

Font::Glyph Font::CreateGlyphFromChar(SDL_Renderer *renderer,
                                      const char16_t &ch) {
  auto index = stbtt_FindGlyphIndex(&font, ch);

  return CreateGlyph(renderer, index);
}
