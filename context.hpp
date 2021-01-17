#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include <SDL2/SDL.h>

struct Context {
  SDL_Renderer *renderer;
  SDL_Rect windowBound;
  bool debug;
};

#endif
