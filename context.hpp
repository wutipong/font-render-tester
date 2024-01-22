#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include <SDL2/SDL.h>

struct Context {
  SDL_Rect windowBound{};
  bool debug{false};
  bool debugGlyphBound{true};
  bool debugBaseline{true};
  bool debugCaret{true};
  bool debugAscend{true};
  bool debugDescend{true};
  bool showTextEditor{true};
};
#endif
