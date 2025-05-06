#pragma once

#include <SDL3/SDL.h>
#include <harfbuzz/hb.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "debug_settings.hpp"
#include <functional>
#include <hb-ot.h>
#include <iterator>
#include <magic_enum/magic_enum_containers.hpp>
#include <map>
#include <string>
#include <vector>

class Font;

struct Glyph {
  SDL_Texture *texture{nullptr};
  SDL_Rect bound{};
  int advance = 0;
};

constexpr inline float FTPosToFloat(const FT_Pos &value) {
  return static_cast<float>(value) / 64.0f;
}
constexpr inline float HBPosToFloat(const hb_position_t &value) {
  return static_cast<float>(value) / 64.0f;
}

enum class VariationAxis {
  Italic,
  OpticalSize,
  Slant,
  Weight,
  Width,
};

struct AxisInfo {
  float min;
  float max;
  float defaultValue;
};

class Font {
public:
  static bool Init();
  static void CleanUp();

  Font();
  Font(const Font &f);

  Font &operator=(const Font &f);

  ~Font();

  bool LoadFile(const std::string &path);
  bool Load(const std::vector<char> &data);

  void Invalidate();

  void SetFontSize(const int &size);

  std::string GetFamilyName() const { return family; }
  std::string GetSubFamilyName() const { return subFamily; }

  bool IsValid() const { return ftFace != nullptr; }

  bool IsVariableFont() const;

  Glyph &GetGlyph(SDL_Renderer *renderer, const int &index);
  Glyph &GetGlyphFromChar(SDL_Renderer *renderer, const char16_t &index);

  float Ascend() const { return ascend; }
  float Descend() const { return descend; }
  float LineGap() const { return linegap; }
  float LineHeight() const { return height; }

  hb_font_t *HbFont() const { return hbFont; }

  magic_enum::containers::array<VariationAxis, std::optional<AxisInfo>>
  GetAxisInfos() const;

  void SetVariationValues(
      const magic_enum::containers::array<VariationAxis, float> &values);

private:
  static FT_Library library;

  bool Initialize();

  Glyph CreateGlyph(SDL_Renderer *renderer, const int &ch);
  Glyph CreateGlyphFromChar(SDL_Renderer *renderer, const char16_t &ch);

  std::vector<char> data{};
  FT_Face ftFace{};
  hb_font_t *hbFont{nullptr};

  int fontSize{-1};

  std::map<unsigned int, Glyph> glyphMap;

  float ascend{0};
  float descend{0};
  float linegap{0};
  float height{0};

  std::string family;
  std::string subFamily;

  magic_enum::containers::array<VariationAxis, std::optional<AxisInfo>>
      axisInfo{};
};
