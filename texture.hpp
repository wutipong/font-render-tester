#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <GL/gl3w.h>
#include <SDL2/SDL.h>

SDL_Texture *CreateTextureFromBitmap(SDL_Renderer *renderer,
                                     SDL_PixelFormat *format,
                                     unsigned char *bitmap, const int &width,
                                     const int &height);

GLuint LoadTextureFromBitmap(unsigned char* bitmap, const int& width, const int& height);

#endif
