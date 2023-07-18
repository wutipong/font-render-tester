#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include <SDL2/SDL.h>
#include <filesystem>

constexpr int MININUM_WIDTH = 800;
constexpr int MINIMUM_HEIGHT = 600;

struct Context {
  SDL_Rect windowBound{
      SDL_WINDOWPOS_UNDEFINED,
      SDL_WINDOWPOS_UNDEFINED,
      MININUM_WIDTH,
      MINIMUM_HEIGHT,
  };

  bool debug{false};
  std::string fontPath{std::filesystem::absolute("fonts").string()};

  SDL_Renderer *renderer{nullptr};

  const SDL_Color debugGlyphBoundColor{0xFF, 0xFF, 0x80, 0xFF};
  const SDL_Color debugLineColor{0xFF, 0x00, 0x00, 0xFF};
  const SDL_Color debugAscendColor{0x40, 0x40, 0xFF, 0x80};
  const SDL_Color debugDescendColor{0x40, 0xFF, 0x40, 0x80};
  const SDL_Color backgroundColor{0x80, 0x80, 0x80, 0xFF};
};

void SaveContext(const Context &ctx, const std::filesystem::path &path);
void LoadContext(Context &ctx, const std::filesystem::path &path);

#endif
