#pragma once

#include <GL/gl3w.h>
#include <SDL2/SDL.h>
#include <harfbuzz/hb.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "context.hpp"
#include <functional>
#include <glm/glm.hpp>
#include <iterator>
#include <map>
#include <string>
#include <vector>

enum class TextRenderEnum { NoShape, LeftToRight, RightToLeft, TopToBottom };
class Font;

typedef std::function<void(Context &ctx, Font &font, const std::string &str,
                           const glm::vec4 &, const hb_script_t &script)>
    TextRenderFunction;

struct Glyph {
  GLuint texture;
  SDL_Rect bound;
  int advance;
  int bearing;
};

class Font {
public:
  static void Init();
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

  Glyph &GetGlyph(const int &index);
  Glyph &GetGlyphFromChar(const char16_t &index);

  float Scale() const { return scale; }
  float Ascend() const { return ascend; }
  float Descend() const { return descend; }
  float LineGap() const { return linegap; }

  hb_font_t *HbFont() const { return hb_font; }

  void SetTextRenderer(const TextRenderEnum &t);

  void RenderText(Context &ctx, const std::string &str, const glm::vec4 &,
                  const hb_script_t &script);

private:
  static FT_Library library;

  bool Initialize();

  Glyph CreateGlyph(const int &ch);
  Glyph CreateGlyphFromChar(const char16_t &ch);

  std::vector<char> data{};
  FT_Face face{};

  int fontSize{-1};
  hb_font_t *hb_font{nullptr};

  std::map<unsigned int, Glyph> glyphMap;

  float scale{0};

  float ascend{0};
  float descend{0};
  float linegap{0};

  std::string family;
  std::string subFamily;

  TextRenderFunction textRenderer;
  TextRenderEnum textRendererEnum{TextRenderEnum::NoShape};
};
