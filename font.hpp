#pragma once

#include <SDL2/SDL.h>
#include <harfbuzz/hb.h>
#include <stb_truetype.h>

#include "context.hpp"
#include <functional>
#include <iterator>
#include <map>
#include <string>
#include <vector>

enum class TextRenderEnum { NoShape, LeftToRight, RightToLeft, TopToBottom };
class Font;

typedef std::function<void(Context &ctx, Font &font, const std::string &str,
                           const SDL_Color &, const hb_script_t &script)>
    TextRenderFunction;

struct Glyph {
  SDL_Texture *texture;
  SDL_Rect bound;
  int advance;
  int bearing;
};

class Font {
public:
  Font();
  Font(const std::string &path);
  Font(const std::vector<unsigned char> &data);
  Font(const Font &f);

  Font &operator=(const Font &f);

  ~Font();

  void Invalidate();

  void SetFontSize(const int &size);

  std::string GetFamilyName() const { return family; }
  std::string GetSubFamilyName() const { return subFamily; }
  bool IsValid() const { return !data.empty(); }

  Glyph& GetGlyph(SDL_Renderer *renderer, const int &index);
  Glyph& GetGlyphFromChar(SDL_Renderer *renderer, const char16_t &index);

  float Scale() const { return scale; }
  float Ascend() const { return ascend; }
  float Descend() const { return descend; }
  float LineGap() const { return linegap; }

  hb_font_t *HbFont() const { return hb_font; }

  void SetTextRenderer(const TextRenderEnum &t);

  void RenderText(Context &ctx, const std::string &str, const SDL_Color &,
                  const hb_script_t &script);

private:
  void Initialize();
  static void LoadFile(const std::string &path,
                       std::vector<unsigned char> &data);

  Glyph CreateGlyph(SDL_Renderer *renderer, const int &ch);
  Glyph CreateGlyphFromChar(SDL_Renderer *renderer, const char16_t &ch);

  std::vector<unsigned char> data{};
  stbtt_fontinfo font;

  int fontSize{-1};
  hb_font_t *hb_font{nullptr};

  std::map<unsigned int, Glyph> glyphMap;

  int rawAscend{0};
  int rawDescend{0};
  int rawLineGap{0};

  float scale{0};

  float ascend{0};
  float descend{0};
  float linegap{0};

  std::string family;
  std::string subFamily;

  TextRenderFunction textRenderer;
  TextRenderEnum textRendererEnum;
};
