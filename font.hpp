#pragma once

#include <SDL2/SDL.h>
#include <harfbuzz/hb.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "context.hpp"
#include <functional>
#include <iterator>
#include <map>
#include <string>
#include <vector>

enum class TextRenderEnum { NoShape, LeftToRight, RightToLeft, TopToBottom };
class Font;

typedef std::function<void(Context &ctx, Font &font, const std::string &str,
                           const SDL_Color &color, const std::string &language,
                           const hb_script_t &script)>
    TextRenderFunction;

struct SizeMetrics {
  int xPixelPerEm;
  int yPixelPerEm;

  float xScale;
  float yScale;

  float ascender;
  float descender;
  float height;
  float max_advance;
};

struct Glyph {
  SDL_Texture *texture;
  SDL_Rect bound;
  int advance;
  int bearing;
};

struct FontAdjustments {
  float ascend;
  float descend;
};

template <typename T> inline T FTPosToNumber(const FT_Pos &value) {
  return static_cast<T>(value) / 64;
}

template <typename T> inline T FTFixedToNumber(const FT_Fixed &value) {
  return static_cast<T>(value) / 16'384;
}

template <typename T> inline T HBPosToNumber(const hb_position_t &value) {
  return static_cast<T>(value) / 64;
}

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
  bool IsValid() const { return face != nullptr; }

  Glyph &GetGlyph(Context &ctx, const int &index);
  Glyph &GetGlyphFromChar(Context &ctx, const char16_t &index);

  float Ascend() const { return ascend; }
  float Descend() const { return descend; }
  float LineGap() const { return linegap; }
  float LineHeight() const { return height; }

  hb_font_t *HbFont() const { return hb_font; }

  void SetTextRenderer(const TextRenderEnum &t);

  void RenderText(Context &ctx, const std::string &str, const SDL_Color &color,
                  const std::string &language, const hb_script_t &script);

  const SizeMetrics& GetSizeMetrics() const {return sizeMetrics;}
  FontAdjustments& GetFontAdjustments() { return fontAdjustments;}

private:
  static FT_Library library;

  bool Initialize();

  Glyph CreateGlyph(Context &ctx, const int &ch);
  Glyph CreateGlyphFromChar(Context &ctx, const char16_t &ch);

  std::vector<char> data{};
  FT_Face face{};

  int fontSize{-1};
  hb_font_t *hb_font{nullptr};

  std::map<unsigned int, Glyph> glyphMap;

  SizeMetrics sizeMetrics{};
  FontAdjustments fontAdjustments{};

  float ascend{0};
  float descend{0};
  float linegap{0};
  float height{0};

  std::string family;
  std::string subFamily;

  TextRenderFunction textRenderer;
  TextRenderEnum textRendererEnum{TextRenderEnum::NoShape};
};

SizeMetrics ToSizeMetrics(FT_Size_Metrics &m);