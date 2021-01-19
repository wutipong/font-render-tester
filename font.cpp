#include "font.hpp"
#include <fstream>
#include <streambuf>

#include "text_renderer.hpp"
#include "texture.hpp"
#include <utf8/cpp11.h>

#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>

Font::Font(){};

Font::Font(const std::string &path) : textRenderer(TextRenderNoShape) {
  Font::LoadFile(path, data);
  Initialize();
}

Font::Font(const Font &f) : Font(f.data) {}

Font::Font(const std::vector<unsigned char> &data)
    : data(data.begin(), data.end()), textRenderer(TextRenderNoShape) {
  Initialize();
}

Font &Font::operator=(const Font &f) {
  data = f.data;
  family = "";
  subFamily = "";
  textRenderer = TextRenderNoShape;

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

static std::string ConvertFromFontString(const char *str, const int &length) {
  const char16_t *c16str = reinterpret_cast<const char16_t *>(str);
  std::vector<char16_t> buffer;
  for (int i = 0; i < length / 2; i++) {
    buffer.push_back(SDL_SwapBE16(c16str[i]));
  }

  std::string output;
  utf8::utf16to8(buffer.begin(), buffer.end(), std::back_inserter(output));

  return output;
}

static std::pair<std::string, std::string>
GetFontName(const stbtt_fontinfo &font) {
  int length;
  auto family = ConvertFromFontString(
      stbtt_GetFontNameString(&font, &length, STBTT_PLATFORM_ID_MICROSOFT,
                              STBTT_MS_EID_UNICODE_BMP, STBTT_MS_LANG_ENGLISH,
                              1),
      length);

  auto sub = ConvertFromFontString(
      stbtt_GetFontNameString(&font, &length, STBTT_PLATFORM_ID_MICROSOFT,
                              STBTT_MS_EID_UNICODE_BMP, STBTT_MS_LANG_ENGLISH,
                              2),
      length);

  return {family, sub};
}

void Font::Initialize() {
  if (!IsValid())
    return;

  stbtt_InitFont(&font, reinterpret_cast<unsigned char *>(data.data()), 0);

  hb_blob_t *blob =
      hb_blob_create(reinterpret_cast<char *>(data.data()), data.size(),
                     HB_MEMORY_MODE_READONLY, nullptr, nullptr);
  hb_face_t *hb_face = hb_face_create(blob, 0);
  hb_blob_destroy(blob);

  hb_font = hb_font_create(hb_face);

  stbtt_GetFontVMetrics(&font, &rawAscend, &rawDescend, &rawLineGap);
  Invalidate();
  fontSize = -1;

  auto names = GetFontName(font);
  family = names.first;
  subFamily = names.second;
}

void Font::Invalidate() { glyphMap.clear(); }

void Font::SetFontSize(const int &size) {
  if (!IsValid())
    return;

  if (fontSize == size) {
    return;
  }

  fontSize = size;
  Invalidate();

  scale = stbtt_ScaleForPixelHeight(&font, fontSize);
  ascend = roundf(scale * rawAscend);
  descend = roundf(scale * rawDescend);
  linegap = roundf(scale * rawLineGap);
}

void Font::LoadFile(const std::string &path, std::vector<unsigned char> &data) {
  std::basic_ifstream<unsigned char> file(path, std::ios::binary);
  data =
      std::vector<unsigned char>{std::istreambuf_iterator<unsigned char>(file),
                                 std::istreambuf_iterator<unsigned char>()};
  file.close();
}

Glyph Font::CreateGlyph(SDL_Renderer *renderer, const int &index) {
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

Glyph Font::CreateGlyphFromChar(SDL_Renderer *renderer, const char16_t &ch) {
  auto index = stbtt_FindGlyphIndex(&font, ch);

  return CreateGlyph(renderer, index);
}

Glyph& Font::GetGlyph(SDL_Renderer *renderer, const int &index) {
  auto iter = glyphMap.find(index);
  if (iter == glyphMap.end()) {
    Glyph g = CreateGlyph(renderer, index);
    auto [i, success] = glyphMap.insert({index, g});

    iter = i;
  }

  return iter->second;
}

Glyph& Font::GetGlyphFromChar(SDL_Renderer *renderer, const char16_t &ch) {
  auto iter = glyphMap.find(ch);
  if (iter == glyphMap.end()) {
    Glyph g = CreateGlyphFromChar(renderer, ch);
    auto [i, success] = glyphMap.insert({ch, g});

    iter = i;
  }

  return iter->second;
}

void Font::SetTextRenderer(const TextRenderEnum &t) {
  if (textRendererEnum == t)
    return;

  textRendererEnum = t;
  switch (textRendererEnum) {
  case TextRenderEnum::NoShape:
    textRenderer = TextRenderNoShape;
    break;
  case TextRenderEnum::LeftToRight:
    textRenderer = TextRenderLeftToRight;
    break;
  case TextRenderEnum::RightToLeft:
    textRenderer = TextRenderRightToLeft;
    break;
  case TextRenderEnum::TopToBottom:
    textRenderer = TextRenderTopToBottom;
    break;
  }
  Invalidate();
}

void Font::RenderText(Context &ctx, const std::string &str,
                      const SDL_Color &color, const hb_script_t &script) {
  if (!IsValid())
    return;
  textRenderer(ctx, *this, str, color, script);
}
