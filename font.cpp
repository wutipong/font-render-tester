#include "font.hpp"

#include <utf8cpp/utf8.h>

#include "io_util.hpp"
#include "text_renderer.hpp"
#include "texture.hpp"

#include FT_SFNT_NAMES_H
#include FT_BITMAP_H
#include FT_MULTIPLE_MASTERS_H

#include <harfbuzz/hb-ft.h>
#include <spdlog/spdlog.h>

namespace {

constexpr magic_enum::containers::array<VariantAxis, hb_tag_t> AxisTags = {{{
    HB_OT_TAG_VAR_AXIS_ITALIC,
    HB_OT_TAG_VAR_AXIS_OPTICAL_SIZE,
    HB_OT_TAG_VAR_AXIS_SLANT,
    HB_OT_TAG_VAR_AXIS_WEIGHT,
    HB_OT_TAG_VAR_AXIS_WIDTH,
}}};
} // namespace

FT_Library Font::library;

bool Font::Init() {
  auto error = FT_Init_FreeType(&library);
  if (error) {
    std::string message(FT_Error_String(error));
    spdlog::error("FreeType 2 fails to initialize: {}", error);

    return false;
  }

  return true;
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

  Invalidate();

  FT_Done_Face(face);
}

bool Font::LoadFile(const std::string &path) {
  data = ::LoadFile<std::vector<char>>(path, std::ios::in | std::ios::binary);
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

bool Font::Initialize() {
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

  Invalidate();
  fontSize = -1;

  family = face->family_name;
  subFamily = face->style_name;

  return true;
}

void Font::Invalidate() {
  for (auto &g : glyphMap) {
    SDL_DestroyTexture(g.second.texture);
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

  auto error = FT_Set_Pixel_Sizes(face, 0, size);
  hb_ft_font_changed(hb_font);

  ascend = FTPosToFloat(face->size->metrics.ascender);
  descend = FTPosToFloat(face->size->metrics.descender);
  height = FTPosToFloat(face->size->metrics.height);
  linegap = height + descend - ascend;
}

Glyph Font::CreateGlyph(SDL_Renderer *renderer, const int &index) {
  int bearing = 0;
  int advance;

  FT_Load_Glyph(face, index, FT_LOAD_RENDER);

  advance = static_cast<int>(FTPosToFloat(face->glyph->advance.x));

  auto width = face->glyph->bitmap.width;
  auto height = face->glyph->bitmap.rows;

  FT_Bitmap bitmap;
  FT_Bitmap_Init(&bitmap);
  FT_Bitmap_Convert(library, &face->glyph->bitmap, &bitmap, 1);

  auto texture = LoadTextureFromBitmap(renderer, bitmap);

  FT_Bitmap_Done(library, &bitmap);

  auto x = face->glyph->bitmap_left;
  auto y = face->glyph->bitmap_top - height;

  SDL_Rect bound{x, static_cast<int>(y), static_cast<int>(width),
                 static_cast<int>(height)};

  return {texture, bound, advance, bearing};
}

Glyph Font::CreateGlyphFromChar(SDL_Renderer *renderer, const char16_t &ch) {
  auto index = FT_Get_Char_Index(face, ch);

  return CreateGlyph(renderer, index);
}

Glyph &Font::GetGlyph(SDL_Renderer *renderer, const int &index) {
  auto iter = glyphMap.find(index);
  if (iter == glyphMap.end()) {
    Glyph g = CreateGlyph(renderer, index);
    auto [i, success] = glyphMap.insert({index, g});

    iter = i;
  }

  return iter->second;
}

Glyph &Font::GetGlyphFromChar(SDL_Renderer *renderer, const char16_t &ch) {
  const auto index = FT_Get_Char_Index(face, ch);

  return Font::GetGlyph(renderer, index);
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

void Font::RenderText(SDL_Renderer *renderer, Context &ctx,
                      const std::string &str, const SDL_Color &color,
                      const std::string &language, const hb_script_t &script) {
  if (!IsValid())
    return;

  textRenderer(renderer, ctx, *this, str, color, language, script);
}

bool Font::IsVariableFont() const {
  if (!hb_font)
    return false;

  auto hb_face = hb_font_get_face(hb_font);
  return hb_ot_var_has_data(hb_face);
}

magic_enum::containers::array<VariantAxis, std::optional<VariantAxisLimit>>
Font::GetVariantAxisLimits() const {
  if (!IsVariableFont()) {
    return {};
  }

  magic_enum::containers::array<VariantAxis, std::optional<VariantAxisLimit>>
      output{};

  auto hb_face = hb_font_get_face(hb_font);
  auto count = hb_ot_var_get_axis_count(hb_face);

  std::vector<hb_ot_var_axis_info_t> hbAxisInfo;
  hbAxisInfo.resize(count);

  hb_ot_var_get_axis_infos(hb_face, 0, &count, hbAxisInfo.data());

  for (auto &info : hbAxisInfo) {
    auto it = std::ranges::find_if( AxisTags, [&info](const hb_tag_t& t) ->bool {
      return info.tag == t;
    });

    if (it != AxisTags.end()) {
      auto tag = magic_enum::enum_cast<VariantAxis>(std::distance(AxisTags.begin(), it));
      output[*tag] = {
          .min = info.min_value,
          .max = info.max_value,
          .defaultValue = info.default_value,
      };
    }
  }

  return output;
}