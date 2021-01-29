#include "font.hpp"

#include "text_renderer.hpp"
#include "texture.hpp"
#include <utf8/cpp11.h>

#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>

#include "io_util.hpp"

Font::Font() : textRenderer(TextRenderNoShape){};

Font &Font::operator=(const Font &f) {
  data = f.data;
  Initialize();

  return *this;
}

Font::Font(const Font &f) : data(f.data) { Initialize(); }

Font::~Font() {
  hb_font_destroy(hb_font);
  hb_font = nullptr;

  for (auto &g : glyphMap) {
    glDeleteTextures(1, &g.second.texture);
  }

  data.clear();
}

bool Font::LoadFile(const std::string &path) {
  ::LoadFile(path, data, std::ios::in | std::ios::binary);
  return Initialize();
}

bool Font::Load(const std::vector<char> &data) {
  Font::data = data;
  return Initialize();
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

bool Font::Initialize() {
  if (!IsValid())
    return true;

  family = "";
  subFamily = "";
  textRenderer = TextRenderNoShape;
  textRendererEnum = TextRenderEnum::NoShape;

  if (stbtt_InitFont(&font, reinterpret_cast<unsigned char *>(data.data()),
                     0) == 0) {
    data.clear();
    return false;
  }

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

  return true;
}

void Font::Invalidate() {
  for (auto &g : glyphMap) {
    glDeleteTextures(1, &g.second.texture);
  }
  glyphMap.clear();
}

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

Glyph Font::CreateGlyph(const int &index) {
  int bearing;
  int advance;

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

  auto texture = LoadTextureFromBitmap(bitmap, width, height);

  delete[] bitmap;

  SDL_Rect bound{x1, -y2, width, height};

  return {texture, bound, advance, bearing};
}

Glyph Font::CreateGlyphFromChar(const char16_t &ch) {
  auto index = stbtt_FindGlyphIndex(&font, ch);

  return CreateGlyph(index);
}

Glyph &Font::GetGlyph(const int &index) {
  auto iter = glyphMap.find(index);
  if (iter == glyphMap.end()) {
    Glyph g = CreateGlyph(index);
    auto [i, success] = glyphMap.insert({index, g});

    iter = i;
  }

  return iter->second;
}

Glyph &Font::GetGlyphFromChar(const char16_t &ch) {
  auto iter = glyphMap.find(ch);
  if (iter == glyphMap.end()) {
    Glyph g = CreateGlyphFromChar(ch);
    auto [i, success] = glyphMap.insert({ch, g});

    iter = i;
  }

  return iter->second;
}

void Font::SetTextRenderer(const TextRenderEnum &t) {
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
}

void Font::RenderText(Context &ctx, const std::string &str,
                      const glm::vec4 &color, const hb_script_t &script) {
  if (!IsValid())
    return;
  textRenderer(ctx, *this, str, color, script);
}
