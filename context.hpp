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
  bool debugGlyphBound{true};
  bool debugBaseline{true};
  bool debugCaret{true};
  bool debugAscend{true};
  bool debugDescend{true};

  std::string fontPath{std::filesystem::absolute("fonts").string()};

  SDL_Renderer *renderer{nullptr};
};

void SaveContext(const Context &ctx, const std::filesystem::path &path);
void LoadContext(Context &ctx, const std::filesystem::path &path);

#endif
