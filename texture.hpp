#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <SDL2/SDL.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include <stb_truetype.h>

SDL_Texture *CreateTextureFromFT_Bitmap(SDL_Renderer *renderer,
                                        const FT_Bitmap &bitmap,
                                        const SDL_Color &color);

SDL_Texture *CreateTextureFromCodePoint(SDL_Renderer *renderer,
                                        const stbtt_fontinfo &info,
                                        wchar_t &codepoint, const float &scale,
                                        const SDL_Color &color, int &advance,
                                        SDL_Rect &dst);
SDL_Texture *CreateTextureFromIndex(SDL_Renderer *renderer,
                                    const stbtt_fontinfo &info,
                                    const int &index, const float &scale,
                                    const SDL_Color &color, int &advance,
                                    SDL_Rect &dst);
#endif