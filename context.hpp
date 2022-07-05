#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include <SDL2/SDL.h>
#include <filesystem>
#include <glm/glm.hpp>

constexpr int WIDTH = 800;
constexpr int HEIGHT = 600;

struct Context {
  SDL_Rect windowBound = {SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                          WIDTH, HEIGHT};
  bool debug = false;
  std::string fontPath = std::filesystem::absolute("fonts").string();

  glm::vec4 debugGlyphBoundColor = {1.0f, 1.0f, 0.5f, 1.0f};
  glm::vec4 debugLineColor = {1.0f, 0.0f, 0.0f, 1.0f};
  glm::vec4 debugAscendColor = {0.25f, 0.25f, 1.0f, 1.0f};
  glm::vec4 debugDescendColor = {1.0f, 0.25f, 0.25f, 1.0f};
  glm::vec4 backgroundColor = {1 / 3.0f, 0.5f, 2 / 3.0f, 1.0f};
};

void SaveContext(const Context &ctx, const std::filesystem::path &path);
void LoadContext(Context &ctx, const std::filesystem::path &path);

#endif
