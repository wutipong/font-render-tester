#include "texture.hpp"

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
