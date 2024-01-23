#include "texture.hpp"
#include <array>
constexpr std::array<SDL_Color, 256> glyphPaletteColor();

SDL_Texture *LoadTextureFromBitmap(SDL_Renderer *renderer, FT_Bitmap &bitmap) {
  if (bitmap.width == 0 || bitmap.rows == 0) {
    return nullptr;
  }

  auto glyphPalette = glyphPaletteColor();

  auto *palette = SDL_AllocPalette(glyphPalette.size());
  SDL_SetPaletteColors(palette, glyphPalette.data(), 0, glyphPalette.size());

  auto *surface = SDL_CreateRGBSurfaceWithFormatFrom(
      bitmap.buffer, static_cast<int>(bitmap.width),
      static_cast<int>(bitmap.rows), 8, static_cast<int>(bitmap.pitch),
      SDL_PIXELFORMAT_INDEX8);

  SDL_SetSurfacePalette(surface, palette);

  auto *texture = SDL_CreateTextureFromSurface(renderer, surface);
  SDL_FreeSurface(surface);
  SDL_FreePalette(palette);

  SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

  return texture;
}

constexpr std::array<SDL_Color, 256> glyphPaletteColor() {
  return {
      SDL_Color{0xFF, 0xFF, 0xFF, 0x00}, SDL_Color{0xFF, 0xFF, 0xFF, 0x01},
      SDL_Color{0xFF, 0xFF, 0xFF, 0x02}, SDL_Color{0xFF, 0xFF, 0xFF, 0x03},
      SDL_Color{0xFF, 0xFF, 0xFF, 0x04}, SDL_Color{0xFF, 0xFF, 0xFF, 0x05},
      SDL_Color{0xFF, 0xFF, 0xFF, 0x06}, SDL_Color{0xFF, 0xFF, 0xFF, 0x07},
      SDL_Color{0xFF, 0xFF, 0xFF, 0x08}, SDL_Color{0xFF, 0xFF, 0xFF, 0x09},
      SDL_Color{0xFF, 0xFF, 0xFF, 0x0A}, SDL_Color{0xFF, 0xFF, 0xFF, 0x0B},
      SDL_Color{0xFF, 0xFF, 0xFF, 0x0C}, SDL_Color{0xFF, 0xFF, 0xFF, 0x0D},
      SDL_Color{0xFF, 0xFF, 0xFF, 0x0E}, SDL_Color{0xFF, 0xFF, 0xFF, 0x0F},

      SDL_Color{0xFF, 0xFF, 0xFF, 0x10}, SDL_Color{0xFF, 0xFF, 0xFF, 0x11},
      SDL_Color{0xFF, 0xFF, 0xFF, 0x12}, SDL_Color{0xFF, 0xFF, 0xFF, 0x13},
      SDL_Color{0xFF, 0xFF, 0xFF, 0x14}, SDL_Color{0xFF, 0xFF, 0xFF, 0x15},
      SDL_Color{0xFF, 0xFF, 0xFF, 0x16}, SDL_Color{0xFF, 0xFF, 0xFF, 0x17},
      SDL_Color{0xFF, 0xFF, 0xFF, 0x18}, SDL_Color{0xFF, 0xFF, 0xFF, 0x19},
      SDL_Color{0xFF, 0xFF, 0xFF, 0x1A}, SDL_Color{0xFF, 0xFF, 0xFF, 0x1B},
      SDL_Color{0xFF, 0xFF, 0xFF, 0x1C}, SDL_Color{0xFF, 0xFF, 0xFF, 0x1D},
      SDL_Color{0xFF, 0xFF, 0xFF, 0x1E}, SDL_Color{0xFF, 0xFF, 0xFF, 0x1F},

      SDL_Color{0xFF, 0xFF, 0xFF, 0x00}, SDL_Color{0xFF, 0xFF, 0xFF, 0x21},
      SDL_Color{0xFF, 0xFF, 0xFF, 0x22}, SDL_Color{0xFF, 0xFF, 0xFF, 0x23},
      SDL_Color{0xFF, 0xFF, 0xFF, 0x24}, SDL_Color{0xFF, 0xFF, 0xFF, 0x25},
      SDL_Color{0xFF, 0xFF, 0xFF, 0x26}, SDL_Color{0xFF, 0xFF, 0xFF, 0x27},
      SDL_Color{0xFF, 0xFF, 0xFF, 0x28}, SDL_Color{0xFF, 0xFF, 0xFF, 0x29},
      SDL_Color{0xFF, 0xFF, 0xFF, 0x2A}, SDL_Color{0xFF, 0xFF, 0xFF, 0x2B},
      SDL_Color{0xFF, 0xFF, 0xFF, 0x2C}, SDL_Color{0xFF, 0xFF, 0xFF, 0x2D},
      SDL_Color{0xFF, 0xFF, 0xFF, 0x2E}, SDL_Color{0xFF, 0xFF, 0xFF, 0x2F},

      SDL_Color{0xFF, 0xFF, 0xFF, 0x30}, SDL_Color{0xFF, 0xFF, 0xFF, 0x31},
      SDL_Color{0xFF, 0xFF, 0xFF, 0x32}, SDL_Color{0xFF, 0xFF, 0xFF, 0x33},
      SDL_Color{0xFF, 0xFF, 0xFF, 0x34}, SDL_Color{0xFF, 0xFF, 0xFF, 0x35},
      SDL_Color{0xFF, 0xFF, 0xFF, 0x36}, SDL_Color{0xFF, 0xFF, 0xFF, 0x37},
      SDL_Color{0xFF, 0xFF, 0xFF, 0x38}, SDL_Color{0xFF, 0xFF, 0xFF, 0x39},
      SDL_Color{0xFF, 0xFF, 0xFF, 0x3A}, SDL_Color{0xFF, 0xFF, 0xFF, 0x3B},
      SDL_Color{0xFF, 0xFF, 0xFF, 0x3C}, SDL_Color{0xFF, 0xFF, 0xFF, 0x3D},
      SDL_Color{0xFF, 0xFF, 0xFF, 0x3E}, SDL_Color{0xFF, 0xFF, 0xFF, 0x3F},

      SDL_Color{0xFF, 0xFF, 0xFF, 0x40}, SDL_Color{0xFF, 0xFF, 0xFF, 0x41},
      SDL_Color{0xFF, 0xFF, 0xFF, 0x42}, SDL_Color{0xFF, 0xFF, 0xFF, 0x43},
      SDL_Color{0xFF, 0xFF, 0xFF, 0x44}, SDL_Color{0xFF, 0xFF, 0xFF, 0x45},
      SDL_Color{0xFF, 0xFF, 0xFF, 0x46}, SDL_Color{0xFF, 0xFF, 0xFF, 0x47},
      SDL_Color{0xFF, 0xFF, 0xFF, 0x48}, SDL_Color{0xFF, 0xFF, 0xFF, 0x49},
      SDL_Color{0xFF, 0xFF, 0xFF, 0x4A}, SDL_Color{0xFF, 0xFF, 0xFF, 0x4B},
      SDL_Color{0xFF, 0xFF, 0xFF, 0x4C}, SDL_Color{0xFF, 0xFF, 0xFF, 0x4D},
      SDL_Color{0xFF, 0xFF, 0xFF, 0x4E}, SDL_Color{0xFF, 0xFF, 0xFF, 0x4F},

      SDL_Color{0xFF, 0xFF, 0xFF, 0x50}, SDL_Color{0xFF, 0xFF, 0xFF, 0x51},
      SDL_Color{0xFF, 0xFF, 0xFF, 0x52}, SDL_Color{0xFF, 0xFF, 0xFF, 0x53},
      SDL_Color{0xFF, 0xFF, 0xFF, 0x54}, SDL_Color{0xFF, 0xFF, 0xFF, 0x55},
      SDL_Color{0xFF, 0xFF, 0xFF, 0x56}, SDL_Color{0xFF, 0xFF, 0xFF, 0x57},
      SDL_Color{0xFF, 0xFF, 0xFF, 0x58}, SDL_Color{0xFF, 0xFF, 0xFF, 0x59},
      SDL_Color{0xFF, 0xFF, 0xFF, 0x5A}, SDL_Color{0xFF, 0xFF, 0xFF, 0x5B},
      SDL_Color{0xFF, 0xFF, 0xFF, 0x5C}, SDL_Color{0xFF, 0xFF, 0xFF, 0x5D},
      SDL_Color{0xFF, 0xFF, 0xFF, 0x5E}, SDL_Color{0xFF, 0xFF, 0xFF, 0x5F},

      SDL_Color{0xFF, 0xFF, 0xFF, 0x60}, SDL_Color{0xFF, 0xFF, 0xFF, 0x61},
      SDL_Color{0xFF, 0xFF, 0xFF, 0x62}, SDL_Color{0xFF, 0xFF, 0xFF, 0x63},
      SDL_Color{0xFF, 0xFF, 0xFF, 0x64}, SDL_Color{0xFF, 0xFF, 0xFF, 0x65},
      SDL_Color{0xFF, 0xFF, 0xFF, 0x66}, SDL_Color{0xFF, 0xFF, 0xFF, 0x67},
      SDL_Color{0xFF, 0xFF, 0xFF, 0x68}, SDL_Color{0xFF, 0xFF, 0xFF, 0x69},
      SDL_Color{0xFF, 0xFF, 0xFF, 0x6A}, SDL_Color{0xFF, 0xFF, 0xFF, 0x6B},
      SDL_Color{0xFF, 0xFF, 0xFF, 0x6C}, SDL_Color{0xFF, 0xFF, 0xFF, 0x6D},
      SDL_Color{0xFF, 0xFF, 0xFF, 0x6E}, SDL_Color{0xFF, 0xFF, 0xFF, 0x6F},

      SDL_Color{0xFF, 0xFF, 0xFF, 0x70}, SDL_Color{0xFF, 0xFF, 0xFF, 0x71},
      SDL_Color{0xFF, 0xFF, 0xFF, 0x72}, SDL_Color{0xFF, 0xFF, 0xFF, 0x73},
      SDL_Color{0xFF, 0xFF, 0xFF, 0x74}, SDL_Color{0xFF, 0xFF, 0xFF, 0x75},
      SDL_Color{0xFF, 0xFF, 0xFF, 0x76}, SDL_Color{0xFF, 0xFF, 0xFF, 0x77},
      SDL_Color{0xFF, 0xFF, 0xFF, 0x78}, SDL_Color{0xFF, 0xFF, 0xFF, 0x79},
      SDL_Color{0xFF, 0xFF, 0xFF, 0x7A}, SDL_Color{0xFF, 0xFF, 0xFF, 0x7B},
      SDL_Color{0xFF, 0xFF, 0xFF, 0x7C}, SDL_Color{0xFF, 0xFF, 0xFF, 0x7D},
      SDL_Color{0xFF, 0xFF, 0xFF, 0x7E}, SDL_Color{0xFF, 0xFF, 0xFF, 0x7F},

      SDL_Color{0xFF, 0xFF, 0xFF, 0x80}, SDL_Color{0xFF, 0xFF, 0xFF, 0x81},
      SDL_Color{0xFF, 0xFF, 0xFF, 0x82}, SDL_Color{0xFF, 0xFF, 0xFF, 0x83},
      SDL_Color{0xFF, 0xFF, 0xFF, 0x84}, SDL_Color{0xFF, 0xFF, 0xFF, 0x85},
      SDL_Color{0xFF, 0xFF, 0xFF, 0x86}, SDL_Color{0xFF, 0xFF, 0xFF, 0x87},
      SDL_Color{0xFF, 0xFF, 0xFF, 0x88}, SDL_Color{0xFF, 0xFF, 0xFF, 0x89},
      SDL_Color{0xFF, 0xFF, 0xFF, 0x8A}, SDL_Color{0xFF, 0xFF, 0xFF, 0x8B},
      SDL_Color{0xFF, 0xFF, 0xFF, 0x8C}, SDL_Color{0xFF, 0xFF, 0xFF, 0x8D},
      SDL_Color{0xFF, 0xFF, 0xFF, 0x8E}, SDL_Color{0xFF, 0xFF, 0xFF, 0x8F},

      SDL_Color{0xFF, 0xFF, 0xFF, 0x90}, SDL_Color{0xFF, 0xFF, 0xFF, 0x91},
      SDL_Color{0xFF, 0xFF, 0xFF, 0x92}, SDL_Color{0xFF, 0xFF, 0xFF, 0x93},
      SDL_Color{0xFF, 0xFF, 0xFF, 0x94}, SDL_Color{0xFF, 0xFF, 0xFF, 0x95},
      SDL_Color{0xFF, 0xFF, 0xFF, 0x96}, SDL_Color{0xFF, 0xFF, 0xFF, 0x97},
      SDL_Color{0xFF, 0xFF, 0xFF, 0x98}, SDL_Color{0xFF, 0xFF, 0xFF, 0x99},
      SDL_Color{0xFF, 0xFF, 0xFF, 0x9A}, SDL_Color{0xFF, 0xFF, 0xFF, 0x9B},
      SDL_Color{0xFF, 0xFF, 0xFF, 0x9C}, SDL_Color{0xFF, 0xFF, 0xFF, 0x9D},
      SDL_Color{0xFF, 0xFF, 0xFF, 0x9E}, SDL_Color{0xFF, 0xFF, 0xFF, 0x9F},

      SDL_Color{0xFF, 0xFF, 0xFF, 0xA0}, SDL_Color{0xFF, 0xFF, 0xFF, 0xA1},
      SDL_Color{0xFF, 0xFF, 0xFF, 0xA2}, SDL_Color{0xFF, 0xFF, 0xFF, 0xA3},
      SDL_Color{0xFF, 0xFF, 0xFF, 0xA4}, SDL_Color{0xFF, 0xFF, 0xFF, 0xA5},
      SDL_Color{0xFF, 0xFF, 0xFF, 0xA6}, SDL_Color{0xFF, 0xFF, 0xFF, 0xA7},
      SDL_Color{0xFF, 0xFF, 0xFF, 0xA8}, SDL_Color{0xFF, 0xFF, 0xFF, 0xA9},
      SDL_Color{0xFF, 0xFF, 0xFF, 0xAA}, SDL_Color{0xFF, 0xFF, 0xFF, 0xAB},
      SDL_Color{0xFF, 0xFF, 0xFF, 0xAC}, SDL_Color{0xFF, 0xFF, 0xFF, 0xAD},
      SDL_Color{0xFF, 0xFF, 0xFF, 0xAE}, SDL_Color{0xFF, 0xFF, 0xFF, 0xAF},

      SDL_Color{0xFF, 0xFF, 0xFF, 0xB0}, SDL_Color{0xFF, 0xFF, 0xFF, 0xB1},
      SDL_Color{0xFF, 0xFF, 0xFF, 0xB2}, SDL_Color{0xFF, 0xFF, 0xFF, 0xB3},
      SDL_Color{0xFF, 0xFF, 0xFF, 0xB4}, SDL_Color{0xFF, 0xFF, 0xFF, 0xB5},
      SDL_Color{0xFF, 0xFF, 0xFF, 0xB6}, SDL_Color{0xFF, 0xFF, 0xFF, 0xB7},
      SDL_Color{0xFF, 0xFF, 0xFF, 0xB8}, SDL_Color{0xFF, 0xFF, 0xFF, 0xB9},
      SDL_Color{0xFF, 0xFF, 0xFF, 0xBA}, SDL_Color{0xFF, 0xFF, 0xFF, 0xBB},
      SDL_Color{0xFF, 0xFF, 0xFF, 0xBC}, SDL_Color{0xFF, 0xFF, 0xFF, 0xBD},
      SDL_Color{0xFF, 0xFF, 0xFF, 0xBE}, SDL_Color{0xFF, 0xFF, 0xFF, 0xBF},

      SDL_Color{0xFF, 0xFF, 0xFF, 0xC0}, SDL_Color{0xFF, 0xFF, 0xFF, 0xC1},
      SDL_Color{0xFF, 0xFF, 0xFF, 0xC2}, SDL_Color{0xFF, 0xFF, 0xFF, 0xC3},
      SDL_Color{0xFF, 0xFF, 0xFF, 0xC4}, SDL_Color{0xFF, 0xFF, 0xFF, 0xC5},
      SDL_Color{0xFF, 0xFF, 0xFF, 0xC6}, SDL_Color{0xFF, 0xFF, 0xFF, 0xC7},
      SDL_Color{0xFF, 0xFF, 0xFF, 0xC8}, SDL_Color{0xFF, 0xFF, 0xFF, 0xC9},
      SDL_Color{0xFF, 0xFF, 0xFF, 0xCA}, SDL_Color{0xFF, 0xFF, 0xFF, 0xCB},
      SDL_Color{0xFF, 0xFF, 0xFF, 0xCC}, SDL_Color{0xFF, 0xFF, 0xFF, 0xCD},
      SDL_Color{0xFF, 0xFF, 0xFF, 0xCE}, SDL_Color{0xFF, 0xFF, 0xFF, 0xCF},

      SDL_Color{0xFF, 0xFF, 0xFF, 0xD0}, SDL_Color{0xFF, 0xFF, 0xFF, 0xD1},
      SDL_Color{0xFF, 0xFF, 0xFF, 0xD2}, SDL_Color{0xFF, 0xFF, 0xFF, 0xD3},
      SDL_Color{0xFF, 0xFF, 0xFF, 0xD4}, SDL_Color{0xFF, 0xFF, 0xFF, 0xD5},
      SDL_Color{0xFF, 0xFF, 0xFF, 0xD6}, SDL_Color{0xFF, 0xFF, 0xFF, 0xD7},
      SDL_Color{0xFF, 0xFF, 0xFF, 0xD8}, SDL_Color{0xFF, 0xFF, 0xFF, 0xD9},
      SDL_Color{0xFF, 0xFF, 0xFF, 0xDA}, SDL_Color{0xFF, 0xFF, 0xFF, 0xDB},
      SDL_Color{0xFF, 0xFF, 0xFF, 0xDC}, SDL_Color{0xFF, 0xFF, 0xFF, 0xDD},
      SDL_Color{0xFF, 0xFF, 0xFF, 0xDE}, SDL_Color{0xFF, 0xFF, 0xFF, 0xDF},

      SDL_Color{0xFF, 0xFF, 0xFF, 0xE0}, SDL_Color{0xFF, 0xFF, 0xFF, 0xE1},
      SDL_Color{0xFF, 0xFF, 0xFF, 0xE2}, SDL_Color{0xFF, 0xFF, 0xFF, 0xE3},
      SDL_Color{0xFF, 0xFF, 0xFF, 0xE4}, SDL_Color{0xFF, 0xFF, 0xFF, 0xE5},
      SDL_Color{0xFF, 0xFF, 0xFF, 0xE6}, SDL_Color{0xFF, 0xFF, 0xFF, 0xE7},
      SDL_Color{0xFF, 0xFF, 0xFF, 0xE8}, SDL_Color{0xFF, 0xFF, 0xFF, 0xE9},
      SDL_Color{0xFF, 0xFF, 0xFF, 0xEA}, SDL_Color{0xFF, 0xFF, 0xFF, 0xEB},
      SDL_Color{0xFF, 0xFF, 0xFF, 0xEC}, SDL_Color{0xFF, 0xFF, 0xFF, 0xED},
      SDL_Color{0xFF, 0xFF, 0xFF, 0xEE}, SDL_Color{0xFF, 0xFF, 0xFF, 0xEF},

      SDL_Color{0xFF, 0xFF, 0xFF, 0xF0}, SDL_Color{0xFF, 0xFF, 0xFF, 0xF1},
      SDL_Color{0xFF, 0xFF, 0xFF, 0xF2}, SDL_Color{0xFF, 0xFF, 0xFF, 0xF3},
      SDL_Color{0xFF, 0xFF, 0xFF, 0xF4}, SDL_Color{0xFF, 0xFF, 0xFF, 0xF5},
      SDL_Color{0xFF, 0xFF, 0xFF, 0xF6}, SDL_Color{0xFF, 0xFF, 0xFF, 0xF7},
      SDL_Color{0xFF, 0xFF, 0xFF, 0xF8}, SDL_Color{0xFF, 0xFF, 0xFF, 0xF9},
      SDL_Color{0xFF, 0xFF, 0xFF, 0xFA}, SDL_Color{0xFF, 0xFF, 0xFF, 0xFB},
      SDL_Color{0xFF, 0xFF, 0xFF, 0xFC}, SDL_Color{0xFF, 0xFF, 0xFF, 0xFD},
      SDL_Color{0xFF, 0xFF, 0xFF, 0xFE}, SDL_Color{0xFF, 0xFF, 0xFF, 0xFF},
  };
}