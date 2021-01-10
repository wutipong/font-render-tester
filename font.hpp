#pragma once

#include <SDL2/SDL.h>
#include <harfbuzz/hb.h>
#include <stb_truetype.h>

#include <iterator>
#include <map>
#include <string>
#include <vector>

class Font {
public:
  Font();
  Font(const std::string &path);
  Font(const std::vector<unsigned char> &data);
  Font(const Font &f);

  Font &operator=(const Font &f);

  ~Font();

  void Invalidate();

  void DrawTextWithoutShaping(SDL_Renderer *renderer, const std::string &str,
                              const SDL_Color &color);

  void DrawTextWithShaping(SDL_Renderer *renderer, const std::string &str,
                           const SDL_Color &color,
                           const hb_direction_t &direction = HB_DIRECTION_LTR,
                           const hb_script_t &script = HB_SCRIPT_COMMON);

  void SetFontSize(const int &size);

  std::string GetFamilyName() const { return family; }
  std::string GetSubFamilyName() const { return subFamily; }

private:
  void Initialize();
  static void LoadFile(const std::string &path,
                       std::vector<unsigned char> &data);

  struct Glyph {
    SDL_Texture *texture;
    SDL_Rect bound;
    int advance;
    int bearing;
  };

  Glyph CreateGlyph(SDL_Renderer *renderer, const int &ch);
  Glyph CreateGlyphFromChar(SDL_Renderer *renderer, const char16_t &ch);

  bool IsValid() const { return !data.empty(); }

  std::vector<unsigned char> data{};
  stbtt_fontinfo font;

  int fontSize{-1};
  hb_font_t *hb_font{nullptr};

  std::map<unsigned int, Glyph> glyphMap;

  int ascend;
  int descend;
  int linegap;

  float scale{0};
  float scaledAscend{0};
  float scaledDescend{0};
  float scaledLinegap{0};

  std::string family;
  std::string subFamily;
};
