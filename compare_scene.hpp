#pragma once

#include <SDL2/SDL.h>

#include <harfbuzz/hb.h>

#include <stb_truetype.h>

#include <array>
#include <string>
#include <vector>

#include "drawt_text.hpp"
#include "scene.hpp"

class CompareScene : public Scene {
public:
  virtual bool Init(const Context &context);
  virtual void Tick(const Context &context);
  virtual void Cleanup(const Context &context);

private:
  static constexpr size_t bufferSize = 256;

  const std::string text = "Test";
  static constexpr char *fontFile = "Sarabun-Regular.ttf";

  int fontSize = 64;
  SDL_Color color = {0, 0, 0, 255};

  hb_font_t *hb_font;

  TextBuffer buffer;

  stbtt_fontinfo fontInfo;
  std::vector<unsigned char> fontData;
};
