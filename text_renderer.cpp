#include "text_renderer.hpp"

#include <utf8/cpp11.h>

void TextRenderNoShape(SDL_Renderer* renderer, const SDL_Rect& bound, Font& font,
    const std::string& str, const SDL_Color& color,
    const hb_script_t& script) {

    if (!font.IsValid())
        return;

    int x = 0, y = font.Ascend();
    auto u16str = utf8::utf8to16(str);

    for (auto& u : u16str) {
        if (u == '\n') {
            x = 0;
            y += -font.Descend() + font.LineGap() + font.Ascend();

            continue;
        }

        auto& g = font.GetGlyphFromChar(renderer, u);
        if (g.texture != nullptr) {
            SDL_SetTextureColorMod(g.texture, color.r, color.g, color.b);

            SDL_Rect dst = g.bound;
            dst.x += x;
            dst.y = y + dst.y;

            SDL_RenderCopy(renderer, g.texture, nullptr, &dst);
        }
        x += g.advance;
    }

}

void TextRenderLeftToRight(SDL_Renderer* renderer, const SDL_Rect& bound, Font& font,
    const std::string& str, const SDL_Color& color,
    const hb_script_t& script) {
    if (!font.IsValid())
        return;

    auto u16str = utf8::utf8to16(str);
    auto lineStart = u16str.begin();
    int y = font.Ascend();

    while (true) {
        auto lineEnd = std::find(lineStart, u16str.end(), '\n');

        std::u16string line(lineStart, lineEnd);

        hb_buffer_t* buffer = hb_buffer_create();
        hb_buffer_set_direction(buffer, HB_DIRECTION_LTR );
        hb_buffer_set_script(buffer, script);

        hb_buffer_add_utf16(buffer,
            reinterpret_cast<const uint16_t*>(line.c_str()),
            line.size(), 0, line.size());

        hb_shape(font.HbFont(), buffer, NULL, 0);

        unsigned int glyph_count = hb_buffer_get_length(buffer);
        hb_glyph_info_t* glyph_infos = hb_buffer_get_glyph_infos(buffer, NULL);
        hb_glyph_position_t* glyph_positions =
            hb_buffer_get_glyph_positions(buffer, NULL);

        int x = 0;

        for (int i = 0; i < glyph_count; i++) {
            auto index = glyph_infos[i].codepoint;

            auto& g = font.GetGlyph(renderer, index);
            if (g.texture != nullptr) {
                SDL_SetTextureColorMod(g.texture, color.r, color.g, color.b);

                SDL_Rect dst = g.bound;
                dst.x += x + (glyph_positions[i].x_offset * font.Scale());
                dst.y = y + dst.y - (glyph_positions[i].y_offset * font.Scale());

                SDL_RenderCopy(renderer, g.texture, nullptr, &dst);
            }
            x += g.advance;
        }
        hb_buffer_destroy(buffer);

        if (lineEnd == u16str.end())
            break;

        lineStart = lineEnd + 1;
        y += -font.Descend() + font.LineGap() + font.Ascend();
    }
}
