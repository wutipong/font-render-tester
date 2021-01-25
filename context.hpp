#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include <SDL2/SDL.h>
#include <filesystem>
#include <glm/glm.hpp>

constexpr int WIDTH = 800;
constexpr int HEIGHT = 600;

struct Context {
  SDL_Renderer *renderer = nullptr;
  SDL_Rect windowBound = {SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                          WIDTH, HEIGHT};
  bool debug = false;
  std::string fontPath = std::filesystem::absolute("fonts").string();

  const SDL_Color debugGlyphBoundColor = {0xff, 0xff, 0x80, 0xff};
  const SDL_Color debugLineColor = {0x80, 0xff, 0x80, 0xff};
  const glm::vec4 backgroundColor = { 0.313725501f, 0.509803951f, 0.666666687f, 1.0f };
};

void SaveContext(const Context& ctx, const std::filesystem::path& path);
void LoadContext(Context& ctx, const std::filesystem::path& path);

#endif
