#ifndef COLORS_HPP
#define COLORS_HPP

#include <SDL2/SDL.h>
#include <array>
#include <imgui.h>

constexpr SDL_Color debugGlyphBoundColor{0xFF, 0xFF, 0x80, 0xFF};
constexpr SDL_Color debugBaselineColor{0xFF, 0x00, 0x00, 0xFF};
constexpr SDL_Color debugCaretColor{0x00, 0xFF, 0xFF, 0xFF};
constexpr SDL_Color debugAscendColor{0x40, 0x40, 0xFF, 0x80};
constexpr SDL_Color debugDescendColor{0x40, 0xFF, 0x40, 0x80};

constexpr SDL_Color defaultForegroundColor{0x00, 0x00, 0x00, 0xFF};
constexpr SDL_Color defaultBackgroundColor{0x80, 0x80, 0x80, 0xFF};

constexpr ImVec4 SDLColorToImVec4(const SDL_Color &color) {
  return ImVec4(static_cast<float>(color.r) / 255.0f,
                static_cast<float>(color.g) / 255.0f,
                static_cast<float>(color.b) / 255.0f,
                static_cast<float>(color.a) / 255.0f);
}

constexpr std::array<float, 4> SDLColorToFloat4(const SDL_Color &color) {
  return {
      static_cast<float>(color.r) / 255.0f,
      static_cast<float>(color.g) / 255.0f,
      static_cast<float>(color.b) / 255.0f,
      static_cast<float>(color.a) / 255.0f,
  };
}

constexpr SDL_Color Float4ToSDLColor(const float &r, const float &g,
                                     const float &b, const float &a = 1.0f) {
  return {
      .r = static_cast<uint8_t>(r * 255.0f),
      .g = static_cast<uint8_t>(g * 255.0f),
      .b = static_cast<uint8_t>(b * 255.0f),
      .a = static_cast<uint8_t>(a * 255.0f),
  };
}

constexpr SDL_Color Float4ToSDLColor(const std::array<float, 4> &f4) {
  return Float4ToSDLColor(f4[0], f4[1], f4[2], f4[3]);
}

#endif