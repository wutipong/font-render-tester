#pragma once
#include <functional>

#include "context.hpp"
#include "font.hpp"
#include <SDL2/SDL.h>
#include <harfbuzz/hb.h>

void InitTextRenderers();
void CleanUpTextRenderers();

void TextRenderNoShape(SDL_Renderer *renderer, Context &ctx, Font &font,
                       const std::string &str, const SDL_Color &color,
                       const std::string &language, const hb_script_t &script);

void TextRenderLeftToRight(SDL_Renderer *renderer, Context &ctx, Font &font,
                           const std::string &str, const SDL_Color &color,
                           const std::string &language,
                           const hb_script_t &script);

void TextRenderRightToLeft(SDL_Renderer *renderer, Context &ctx, Font &font,
                           const std::string &str, const SDL_Color &color,
                           const std::string &language,
                           const hb_script_t &script);

void TextRenderTopToBottom(SDL_Renderer *renderer, Context &ctx, Font &font,
                           const std::string &str, const SDL_Color &color,
                           const std::string &language,
                           const hb_script_t &script);

void DrawGlyph(Context &ctx, const Font &font, const Glyph &g,
               const SDL_Color &color, const int &x, const int &y);

void DrawGlyph(Context &ctx, const Font &font, const Glyph &g,
               const SDL_Color &color, const int &x, const int &y,
               const hb_glyph_position_t &hb_glyph_pos);
