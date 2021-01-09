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
}

void Font::Invalidate() { glyphMap.clear(); }

void Font::DrawTextWithoutShaping(SDL_Renderer *renderer,
                                  const std::string &str,
                                  const SDL_Color &color) {
  if (data.empty())
    return;

  int x = 0, y = ascend;
  auto u16str = utf8::utf8to16(str);

  for (auto &u : u16str) {
    if (u == '\n') {
      x = 0;
      y += descend + linegap + ascend;

      continue;
    }

    auto iter = glyphMap.find(u);
    if (iter == glyphMap.end()) {
      Glyph g = CreateGlyphFromChar(renderer, u);
      auto [iter, success] = glyphMap.insert({u, g});
    }

    auto &g = iter->second;

    SDL_SetTextureColorMod(g.texture, color.r, color.g, color.b);

    SDL_Rect dst = g.bound;
    dst.x += x;
    dst.y = y + dst.y;

    SDL_RenderCopy(renderer, g.texture, nullptr, &dst);
  }
}

void Font::DrawTextWithShaping(SDL_Renderer *renderer, const std::string &str,
                               const SDL_Color &color) {
  if (data.empty())
    return;
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
