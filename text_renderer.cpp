#include "text_renderer.hpp"

#include <utf8/cpp11.h>

#include "font.hpp"
#include <cstring>

static const std::string glyphFrag =
    "#version 450 core \n"
    "out vec4 FragColor; "
    "in vec2 TexCoord; "

    "layout(binding = 0) uniform sampler2D alphaMap; "
    "uniform vec4 glyphColor; "

    "void main() { "
    "    float alpha = texture(alphaMap, TexCoord).r; "

    "    FragColor = glyphColor; "
    "    FragColor.a = glyphColor.a * alpha; "
    "}";

static const std::string glyphVert =
    "#version 450 core \n"

    "layout(location = 0) in vec3 aPos; "
    "layout(location = 1) in vec2 aTexCoord; "

    "layout(location = 0) uniform vec2 screen; "
    "layout(location = 1) uniform mat4 glyphTransform; "
    "layout(location = 2) uniform mat4 transform; "

    "out vec2 TexCoord; "

    "void main() { "
    "    vec4 pos = glyphTransform * vec4(aPos, 1.0f); "
    "    pos = transform * pos; "

    "    vec2 halfScreen = screen / 2; "

    "    gl_Position = vec4((pos.x - halfScreen.x) / halfScreen.x, "
    "        (pos.y - halfScreen.y) / halfScreen.y, pos.z, pos.w); "

    "    TexCoord = aTexCoord; "
    "} ";

static const float vertices[] = {0.0f, 0.0f, 0.0f, 0.0f, 1.0f,

                                 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,

                                 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,

                                 0.0f, 1.0f, 0.0f, 0.0f, 0.0f};

static const GLubyte indices[] = {0, 1, 2,

                                  2, 3, 0};

static GLuint program;
static GLuint vertexShader;
static GLuint fragmentShader;
static GLuint colorUniform;

static GLuint ReadShader(const GLenum &shaderType, const std::string &src) {
  auto shader = glCreateShader(shaderType);

  GLint size = src.size();
  const GLchar *code = src.c_str();

  glShaderSource(shader, 1, &code, &size);
  glCompileShader(shader);

  GLint result;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &result);

  if (result == GL_FALSE) {
    GLint length;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

    std::vector<GLchar> log;
    log.reserve(length);

    glGetShaderInfoLog(shader, length, nullptr, log.data());

    SDL_Log("Compile results: %s", log.data());

    throw "error";
  }

  return shader;
}

void InitTextRenderers() {
  vertexShader = ReadShader(GL_VERTEX_SHADER, glyphVert);
  fragmentShader = ReadShader(GL_FRAGMENT_SHADER, glyphFrag);

  program = glCreateProgram();

  glAttachShader(program, vertexShader);
  glAttachShader(program, fragmentShader);
  glLinkProgram(program);

  glUseProgram(program);
  colorUniform = glGetUniformLocation(program, "glyphColor");
}

void CleanUpTextRenderers() {
  glDeleteProgram(program);
  glDeleteShader(fragmentShader);
  glDeleteShader(vertexShader);
}

void TextRenderNoShape(Context &ctx, Font &font, const std::string &str,
                       const glm::vec4& color, const hb_script_t &script) {
  if (!font.IsValid())
    return;

  int x = 0, y = font.Ascend();
  auto u16str = utf8::utf8to16(str);

  const auto lineHeight = -font.Descend() + font.LineGap() + font.Ascend();
  for (auto &u : u16str) {
    if (u == '\n') {
      x = 0;
      y += lineHeight;

      continue;
    }
    if (x == 0 && ctx.debug) {
      SDL_SetRenderDrawColor(ctx.renderer, ctx.debugLineColor.r,
                             ctx.debugLineColor.g, ctx.debugLineColor.b,
                             ctx.debugLineColor.a);
      SDL_RenderDrawLine(ctx.renderer, 0, y, ctx.windowBound.w, y);
    }

    auto &g = font.GetGlyphFromChar(u);
    DrawGlyph(ctx, font, g, color, x, y);
    x += g.advance;
  }
}

void TextRenderLeftToRight(Context &ctx, Font &font, const std::string &str,
                           const glm::vec4& color, const hb_script_t &script) {
  if (!font.IsValid())
    return;

  auto u16str = utf8::utf8to16(str);
  auto lineStart = u16str.begin();
  int y = font.Ascend();
  const auto lineHeight = -font.Descend() + font.LineGap() + font.Ascend();

  while (true) {
    auto lineEnd = std::find(lineStart, u16str.end(), '\n');

    std::u16string line(lineStart, lineEnd);

    hb_buffer_t *buffer = hb_buffer_create();
    hb_buffer_set_direction(buffer, HB_DIRECTION_LTR);
    hb_buffer_set_script(buffer, script);

    hb_buffer_add_utf16(buffer,
                        reinterpret_cast<const uint16_t *>(line.c_str()),
                        line.size(), 0, line.size());

    hb_shape(font.HbFont(), buffer, NULL, 0);

    unsigned int glyph_count = hb_buffer_get_length(buffer);
    hb_glyph_info_t *glyph_infos = hb_buffer_get_glyph_infos(buffer, NULL);
    hb_glyph_position_t *glyph_positions =
        hb_buffer_get_glyph_positions(buffer, NULL);

    int x = 0;

    for (int i = 0; i < glyph_count; i++) {
      auto index = glyph_infos[i].codepoint;

      auto &g = font.GetGlyph(index);
      DrawGlyph(ctx, font, g, color, x, y, glyph_positions[i]);
      x += g.advance;
    }
    hb_buffer_destroy(buffer);

    if (ctx.debug) {
      SDL_SetRenderDrawColor(ctx.renderer, ctx.debugLineColor.r,
                             ctx.debugLineColor.g, ctx.debugLineColor.b,
                             ctx.debugLineColor.a);
      SDL_RenderDrawLine(ctx.renderer, 0, y, ctx.windowBound.w, y);
    }

    if (lineEnd == u16str.end())
      break;

    lineStart = lineEnd + 1;
    y += lineHeight;
  }
}

void TextRenderRightToLeft(Context &ctx, Font &font, const std::string &str,
                           const glm::vec4 &color, const hb_script_t &script) {
  if (!font.IsValid())
    return;

  auto u16str = utf8::utf8to16(str);
  auto lineStart = u16str.begin();

  hb_font_extents_t extents;
  hb_font_get_extents_for_direction(font.HbFont(), HB_DIRECTION_RTL, &extents);

  int y = roundf(extents.ascender * font.Scale());

  while (true) {
    if (ctx.debug) {
      SDL_SetRenderDrawColor(ctx.renderer, ctx.debugLineColor.r,
                             ctx.debugLineColor.g, ctx.debugLineColor.b,
                             ctx.debugLineColor.a);
      SDL_RenderDrawLine(ctx.renderer, 0, y, ctx.windowBound.w, y);
    }
    auto lineEnd = std::find(lineStart, u16str.end(), '\n');

    std::u16string line(lineStart, lineEnd);

    hb_buffer_t *buffer = hb_buffer_create();
    hb_buffer_set_direction(buffer, HB_DIRECTION_LTR);
    hb_buffer_set_script(buffer, script);

    hb_buffer_add_utf16(buffer,
                        reinterpret_cast<const uint16_t *>(line.c_str()),
                        line.size(), 0, line.size());

    hb_shape(font.HbFont(), buffer, NULL, 0);

    unsigned int glyph_count = hb_buffer_get_length(buffer);
    hb_glyph_info_t *glyph_infos = hb_buffer_get_glyph_infos(buffer, NULL);
    hb_glyph_position_t *glyph_positions =
        hb_buffer_get_glyph_positions(buffer, NULL);

    int x = ctx.windowBound.w;
    const auto lineHeight = -font.Descend() + font.LineGap() + font.Ascend();

    for (int i = 0; i < glyph_count; i++) {
      auto index = glyph_infos[i].codepoint;

      auto &g = font.GetGlyph(index);
      DrawGlyph(ctx, font, g, color, x, y, glyph_positions[i]);
      x -= glyph_positions[i].x_advance * font.Scale();
    }
    hb_buffer_destroy(buffer);

    if (lineEnd == u16str.end())
      break;

    lineStart = lineEnd + 1;
    y += lineHeight;
  }
}

void TextRenderTopToBottom(Context &ctx, Font &font, const std::string &str,
                           const glm::vec4 &color, const hb_script_t &script) {
  if (!font.IsValid())
    return;

  hb_font_extents_t extents;
  hb_font_get_extents_for_direction(font.HbFont(), HB_DIRECTION_TTB, &extents);

  float ascend = roundf(extents.ascender * font.Scale());
  float descend = roundf(extents.descender * font.Scale());
  float linegap = roundf(extents.descender * font.Scale());

  auto u16str = utf8::utf8to16(str);
  auto lineStart = u16str.begin();
  int x = ctx.windowBound.w - ascend;

  const auto lineWidth = -ascend + descend + linegap;

  while (true) {
    auto lineEnd = std::find(lineStart, u16str.end(), '\n');

    std::u16string line(lineStart, lineEnd);

    hb_buffer_t *buffer = hb_buffer_create();
    hb_buffer_set_direction(buffer, HB_DIRECTION_TTB);
    hb_buffer_set_script(buffer, script);

    hb_buffer_add_utf16(buffer,
                        reinterpret_cast<const uint16_t *>(line.c_str()),
                        line.size(), 0, line.size());

    hb_shape(font.HbFont(), buffer, NULL, 0);

    unsigned int glyph_count = hb_buffer_get_length(buffer);
    hb_glyph_info_t *glyph_infos = hb_buffer_get_glyph_infos(buffer, NULL);
    hb_glyph_position_t *glyph_positions =
        hb_buffer_get_glyph_positions(buffer, NULL);

    int y = 0;

    for (int i = 0; i < glyph_count; i++) {
      auto index = glyph_infos[i].codepoint;

      auto &g = font.GetGlyph(index);
      DrawGlyph(ctx, font, g, color, x, y, glyph_positions[i]);

      y -= roundf(glyph_positions[i].y_advance * font.Scale());
    }
    hb_buffer_destroy(buffer);

    if (ctx.debug) {
      SDL_SetRenderDrawColor(ctx.renderer, ctx.debugLineColor.r,
                             ctx.debugLineColor.g, ctx.debugLineColor.b,
                             ctx.debugLineColor.a);
      SDL_RenderDrawLine(ctx.renderer, x, 0, x, ctx.windowBound.h);
    }

    if (lineEnd == u16str.end())
      break;

    lineStart = lineEnd + 1;
    x += lineWidth;
  }
}

void DrawGlyph(Context &ctx, const Font &font, const Glyph &g,
               const glm::vec4 &color, const int &x, const int &y) {
  if (g.bound.w == 0 || g.bound.h == 0)
    return;

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


  glUniform4f(colorUniform, color.r, color.g, color.b, color.a);

  SDL_Rect dst = g.bound;
  dst.x += x;
  dst.y = y + dst.y;

  // SDL_RenderCopy(ctx.renderer, g.texture, nullptr, &dst);

  /* if (ctx.debug) {
    SDL_SetRenderDrawColor(
        ctx.renderer, ctx.debugGlyphBoundColor.r, ctx.debugGlyphBoundColor.g,
        ctx.debugGlyphBoundColor.b, ctx.debugGlyphBoundColor.a);
    SDL_RenderDrawRect(ctx.renderer, &dst);
  } */
}

void DrawGlyph(Context &ctx, const Font &font, const Glyph &g,
               const glm::vec4 &color, const int &x, const int &y,
               const hb_glyph_position_t &hb_glyph_pos) {
  if (g.bound.w == 0 || g.bound.h == 0)
    return;

  glUniform4f(colorUniform, color.r, color.g, color.b, color.a);

  SDL_Rect dst = g.bound;
  dst.x += x + (hb_glyph_pos.x_offset * font.Scale());
  dst.y = y + dst.y - (hb_glyph_pos.y_offset * font.Scale());

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // SDL_RenderCopy(ctx.renderer, g.texture, nullptr, &dst);
  /* if (ctx.debug) {
    SDL_SetRenderDrawColor(
        ctx.renderer, ctx.debugGlyphBoundColor.r, ctx.debugGlyphBoundColor.g,
        ctx.debugGlyphBoundColor.b, ctx.debugGlyphBoundColor.a);
    SDL_RenderDrawRect(ctx.renderer, &dst);
  } */

}
