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

typedef std::function<void(
    SDL_Renderer *renderer, Context &ctx, Font &font, const std::string &str,
    const SDL_Color &color, const std::string &language, const hb_script_t &script)>
    TextRenderFunction;

struct Glyph {

  SDL_Texture *texture;
  SDL_Rect bound;
  int advance;
  int bearing;
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
  bool IsValid() const { return face != nullptr; }

  Glyph &GetGlyph(const int &index, SDL_Renderer* renderer);
  Glyph &GetGlyphFromChar(const char16_t &index, SDL_Renderer* renderer);

  float Ascend() const { return ascend; }
  float Descend() const { return descend; }
  float LineGap() const { return linegap; }
  float LineHeight() const { return height; }

  hb_font_t *HbFont() const { return hb_font; }

  void SetTextRenderer(const TextRenderEnum &t);

  void RenderText(SDL_Renderer *renderer, Context &ctx, const std::string &str,
                  const SDL_Color &color, const std::string &language,
                  const hb_script_t &script);

private:
  static FT_Library library;

  bool Initialize();

  Glyph CreateGlyph(SDL_Renderer* renderer, const int &ch);
  Glyph CreateGlyphFromChar(SDL_Renderer* renderer, const char16_t &ch);

  std::vector<char> data{};
  FT_Face face{};

  int fontSize{-1};
  hb_font_t *hb_font{nullptr};

  std::map<unsigned int, Glyph> glyphMap;

  float ascend{0};
  float descend{0};
  float linegap{0};
  float height{0};

  std::string family;
  std::string subFamily;

  TextRenderFunction textRenderer;
  TextRenderEnum textRendererEnum{TextRenderEnum::NoShape};
};
