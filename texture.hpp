#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <GL/gl3w.h>
#include <SDL2/SDL.h>

GLuint LoadTextureFromBitmap(unsigned char* bitmap, const int& width, const int& height);

#endif
