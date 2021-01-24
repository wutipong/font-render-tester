#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include <SDL2/SDL.h>
#include <filesystem>

constexpr int WIDTH = 800;
constexpr int HEIGHT = 600;

struct Context {
  SDL_Renderer *renderer = nullptr;
  SDL_Rect windowBound = {SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                          WIDTH, HEIGHT};
  bool debug = false;
  const SDL_Color debugGlyphBoundColor = {0xff, 0xff, 0x80, 0xff};
  const SDL_Color debugLineColor = {0x80, 0xff, 0x80, 0xff};
  const SDL_Color backgroundColor = {0x50, 0x82, 0xaa, 0xff};
};

void SaveContext(const Context& ctx, const std::filesystem::path& path);
void LoadContext(Context& ctx, const std::filesystem::path& path);

#endif
