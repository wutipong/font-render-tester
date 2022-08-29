#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <SDL2/SDL.h>

#include <freetype/freetype.h>
#include FT_BITMAP_H

SDL_Texture *LoadTextureFromBitmap(SDL_Renderer *renderer, FT_Bitmap &bitmap);

#endif
