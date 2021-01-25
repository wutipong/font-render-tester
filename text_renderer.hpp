#pragma once
#include <functional>

#include "context.hpp"
#include "font.hpp"
#include <SDL2/SDL.h>
#include <harfbuzz/hb.h>
#include <glm/glm.hpp>

void InitTextRenderers();
void CleanUpTextRenderers();

void TextRenderNoShape(Context &ctx, Font &font, const std::string &str,
                       const glm::vec4& color, const hb_script_t &script);

void TextRenderLeftToRight(Context &ctx, Font &font, const std::string &str,
                           const glm::vec4& color, const hb_script_t &script);

void TextRenderRightToLeft(Context &ctx, Font &font, const std::string &str,
                           const glm::vec4& color, const hb_script_t &script);

void TextRenderTopToBottom(Context &ctx, Font &font, const std::string &str,
                           const glm::vec4& color, const hb_script_t &script);

void DrawGlyph(Context& ctx, const Font &font, const Glyph &g,
               const glm::vec4 &color, const int &x, const int &y);

void DrawGlyph(Context& ctx, const Font &font, const Glyph &g,
               const glm::vec4 &color, const int &x, const int &y,
               const hb_glyph_position_t &hb_glyph_pos);
