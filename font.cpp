#include "font.hpp"

#include "text_renderer.hpp"
#include "texture.hpp"
#include <utf8/cpp11.h>

#include "io_util.hpp"

#include FT_SFNT_NAMES_H

#include "harfbuzz/hb-ft.h"

FT_Library Font::library;

void Font::Init() {
  auto error = FT_Init_FreeType(&library);
  if (error) {
    // TODO:: Add error handling.
  }
}

void Font::CleanUp() { FT_Done_FreeType(library); }

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
  FT_Done_Face(face);
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

static std::pair<std::string, std::string> GetFontName(const FT_Face &face) {
  FT_SfntName name;
  FT_Get_Sfnt_Name(face, 0, &name);

  return {std::string(reinterpret_cast<char *>(name.string), name.string_len),
          ""};

  /*

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
    */
}

bool Font::Initialize() {
  if (!IsValid())
    return true;

  family = "";
  subFamily = "";
  textRenderer = TextRenderNoShape;
  textRendererEnum = TextRenderEnum::NoShape;

  auto error = FT_New_Memory_Face(
      library, reinterpret_cast<const FT_Byte *>(data.data()), data.size(), 0,
      &face);

  if (error) {
    data.clear();
    return false;
  }

  hb_font = hb_ft_font_create_referenced(face);

  // stbtt_GetFontVMetrics(&face, &rawAscend, &rawDescend, &rawLineGap);
  Invalidate();
  fontSize = -1;

  auto names = GetFontName(face);
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

  auto error = FT_Set_Pixel_Sizes(face, 0, 16);

  // scale = stbtt_ScaleForPixelHeight(&face, fontSize);
  // ascend = roundf(scale * rawAscend);
  // descend = roundf(scale * rawDescend);
  // linegap = roundf(scale * rawLineGap);
}

Glyph Font::CreateGlyph(const int &index) {
  int bearing;
  int advance;

  // stbtt_GetGlyphHMetrics(&face, index, &advance, &bearing);

  advance = static_cast<int>(roundf(advance * scale));
  bearing = static_cast<int>(roundf(bearing * scale));

  int x1, y1, x2, y2;
  // stbtt_GetGlyphBitmapBox(&face, index, scale, scale, &x1, &y1, &x2, &y2);

  int width = x2 - x1;
  int height = y2 - y1;

  auto bitmap = new unsigned char[width * height];

  // stbtt_MakeGlyphBitmap(&face, bitmap, width, height, width, scale, scale,
  //                      index);

  auto texture = LoadTextureFromBitmap(bitmap, width, height);

  delete[] bitmap;

  SDL_Rect bound{x1, -y2, width, height};

  return {texture, bound, advance, bearing};
}

Glyph Font::CreateGlyphFromChar(const char16_t &ch) {
  auto index = FT_Get_Char_Index(face, ch);

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
