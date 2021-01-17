#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include <SDL2/SDL.h>

struct Context {
  SDL_Renderer *renderer;
  SDL_Rect windowBound;
  bool debug;
  const SDL_Color debugGlyphBoundColor = {0xff, 0xff, 0x80, 0xff};
  const SDL_Color debugLineColor = {0x80, 0xff, 0x80, 0xff};
};

#endif
