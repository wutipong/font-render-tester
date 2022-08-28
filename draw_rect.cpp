#include "draw_rect.hpp"

#include "io_util.hpp"

void InitDrawRect() {
  //DO NOTHING
}

void CleanUpDrawRect() {

}

void DrawRect(const float &x, const float &y, const float &w, const float &h,
              const SDL_Color &color, const int &screenWidth,
              const int &screenHeight, const DrawRectMode &mode) {
  switch (mode) {

  case DrawRectMode::Fill:
    //SDL_FillRect
    break;

  case DrawRectMode::Outline:
    //SDL_DrawRect
    break;
  }

}
