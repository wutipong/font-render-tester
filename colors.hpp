#ifndef COLORS_HPP
#define COLORS_HPP

#include <SDL2/SDL.h>
#include <imgui.h>

constexpr SDL_Color debugGlyphBoundColor{0xFF, 0xFF, 0x80, 0xFF};
constexpr SDL_Color debugBaselineColor{0xFF, 0x00, 0x00, 0xFF};
constexpr SDL_Color debugCaretColor{0x00, 0xFF, 0xFF, 0xFF};
constexpr SDL_Color debugAscendColor{0x40, 0x40, 0xFF, 0x80};
constexpr SDL_Color debugDescendColor{0x40, 0xFF, 0x40, 0x80};
constexpr SDL_Color backgroundColor{0x80, 0x80, 0x80, 0xFF};

constexpr ImVec4 SDLColorToImVec4(const SDL_Color &color) {
  ImVec4 output{
      static_cast<float>(color.r) / 255.0f,
      static_cast<float>(color.g) / 255.0f,
      static_cast<float>(color.b) / 255.0f,
      static_cast<float>(color.a) / 255.0f,
  };

  return output;
}

constexpr auto f4DebugGlyphBoundColor = SDLColorToImVec4(debugGlyphBoundColor);
constexpr auto f4DebugAscendColor = SDLColorToImVec4(debugAscendColor);
constexpr auto f4DebugDescendColor = SDLColorToImVec4(debugDescendColor);
constexpr auto f4DebugBaselineColor = SDLColorToImVec4(debugBaselineColor);
constexpr auto f4DebugCaretColor = SDLColorToImVec4(debugCaretColor);

constexpr SDL_Color Float4ToSDLColor(const float& r, const float& g, const float &b, const float &a = 1.0f ){
  return {
      static_cast<uint8_t>(r * 255.0f),
      static_cast<uint8_t>(g * 255.0f),
      static_cast<uint8_t>(b * 255.0f),
      static_cast<uint8_t>(a * 255.0f),
  };
}
#endif