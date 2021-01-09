#include "texture.hpp"

#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>

SDL_Texture *CreateTextureFromFT_Bitmap(SDL_Renderer *renderer,
                                        const FT_Bitmap &bitmap,
                                        const SDL_Color &color) {
  SDL_Texture *output =
      SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                        SDL_TEXTUREACCESS_STREAMING, bitmap.width, bitmap.rows);

  if (output == nullptr) {
    return output;
  }

  void *buffer;
  int pitch;
  SDL_LockTexture(output, NULL, &buffer, &pitch);

  unsigned char *src_pixels = bitmap.buffer;
  unsigned int *target_pixels = reinterpret_cast<unsigned int *>(buffer);

  SDL_PixelFormat *pixel_format = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888);

  for (int y = 0; y < bitmap.rows; y++) {
    for (int x = 0; x < bitmap.width; x++) {
      int index = (y * bitmap.width) + x;

      unsigned int alpha = src_pixels[index];
      unsigned int pixel_value =
          SDL_MapRGBA(pixel_format, color.r, color.g, color.b, alpha);

      target_pixels[index] = pixel_value;
    }
  }

  SDL_FreeFormat(pixel_format);
  SDL_UnlockTexture(output);

  return output;
}

SDL_Texture *CreateTextureFromCodePoint(SDL_Renderer *renderer,
                                        const stbtt_fontinfo &info,
                                        wchar_t &codepoint, const float &scale,
                                        const SDL_Color &color, int &advance,
                                        SDL_Rect &dst) {
  int bearing;

  auto format = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA32);

  stbtt_GetCodepointHMetrics(&info, static_cast<int>(codepoint), &advance,
                             &bearing);
  advance = static_cast<int>(roundf(advance * scale));
  bearing = static_cast<int>(roundf(bearing * scale));

  int c_x1, c_y1, c_x2, c_y2;
  stbtt_GetCodepointBitmapBox(&info, static_cast<int>(codepoint), scale, scale,
                              &c_x1, &c_y1, &c_x2, &c_y2);
  int y = c_y1;

  int width = c_x2 - c_x1;
  int height = c_y2 - c_y1;

  unsigned char *bitmap =
      new unsigned char[static_cast<size_t>(width) * height];

  stbtt_MakeCodepointBitmap(&info, bitmap, width, height, width, scale, scale,
                            static_cast<int>(codepoint));

  auto texture =
      CreateTextureFromBitmap(renderer, format, bitmap, width, height);

  delete[] bitmap;

  dst = {bearing, static_cast<int>(y), width, height};

  SDL_FreeFormat(format);

  return texture;
}

SDL_Texture *CreateTextureFromIndex(SDL_Renderer *renderer,
                                    const stbtt_fontinfo &info,
                                    const int &index, const float &scale,
                                    const SDL_Color &color, int &advance,
                                    SDL_Rect &dst) {
  int bearing;

  auto format = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA32);

  stbtt_GetGlyphHMetrics(&info, index, &advance, &bearing);

  advance = static_cast<int>(roundf(advance * scale));
  bearing = static_cast<int>(roundf(bearing * scale));

  int c_x1, c_y1, c_x2, c_y2;
  stbtt_GetGlyphBitmapBox(&info, index, scale, scale, &c_x1, &c_y1, &c_x2,
                          &c_y2);
  int y = c_y1;

  int width = c_x2 - c_x1;
  int height = c_y2 - c_y1;

  unsigned char *bitmap =
      new unsigned char[static_cast<size_t>(width) * height];

  stbtt_MakeGlyphBitmap(&info, bitmap, width, height, width, scale, scale,
                        index);

  auto texture =
      CreateTextureFromBitmap(renderer, format, bitmap, width, height);

  delete[] bitmap;

  dst = {c_x1, y, width, height};

  SDL_FreeFormat(format);

  return texture;
}

SDL_Texture *CreateTextureFromBitmap(SDL_Renderer *renderer,
                                     SDL_PixelFormat *format,
                                     unsigned char *bitmap, const int &width,
                                     const int &height) {

  auto surface = SDL_CreateRGBSurfaceWithFormat(
      0, width, height, format->BitsPerPixel, format->format);

  SDL_LockSurface(surface);

  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      unsigned char &alpha = bitmap[y * width + x];

      auto pixel = reinterpret_cast<Uint32 *>(surface->pixels) +
                   (y * (surface->pitch / sizeof(Uint32)) + x);

      *pixel = SDL_MapRGBA(format, 0xff, 0xff, 0xff, alpha);
    }
  }

  SDL_UnlockSurface(surface);

  auto texture = SDL_CreateTextureFromSurface(renderer, surface);
  if (texture != nullptr) {
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
  }

  SDL_FreeSurface(surface);

  return texture;
}
