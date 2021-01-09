#ifndef DRAW_TEXT_HPP
#define DRAW_TEXT_HPP

#include <array>
#include <vector>

#include <SDL2/SDL.h>

#include <harfbuzz/hb.h>
#include <stb_truetype.h>

constexpr size_t bufferSize = 256;

typedef std::array<char, bufferSize> TextBuffer;

void DrawText(const std::array<char, bufferSize> &text, const SDL_Color &color,
              const int &x_start, const int &baseline,
              const stbtt_fontinfo &info, const float &scale,
              SDL_Renderer *renderer);

void DrawTextHarfbuzz(const std::array<char, bufferSize> &text,
                      const SDL_Color &color, const int &x_start,
                      const int &baseline, const stbtt_fontinfo &info,
                      const float &scale, hb_font_t *hb_font,
                      SDL_Renderer *renderer);

#endif