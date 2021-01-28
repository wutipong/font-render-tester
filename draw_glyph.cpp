#include "draw_glyph.hpp"

#include "draw_rect.hpp"

#include <GL/gl3w.h>
#include <glm/gtc/type_ptr.hpp>

static const char vertShaderSrc[] =
    "#version 450 core\n"

    "layout(location = 0) in vec3 aPos;"
    "layout(location = 1) in vec2 aTexCoord;"

    "layout(location = 0) uniform vec2 screen;"
    "layout(location = 1) uniform mat4 glyphTransform;"
    "layout(location = 2) uniform mat4 transform;"
    "layout(location = 3) uniform vec4 color;"

    "out vec2 TexCoord;"
    "out vec4 exColor;"

    "void main() {"
    "    vec4 pos = glyphTransform * vec4(aPos, 1.0f);"
    "    pos = transform * pos;"

    "    vec2 halfScreen = screen / 2;"

    "    gl_Position = vec4((pos.x - halfScreen.x) / halfScreen.x,"
    "                       (pos.y - halfScreen.y) / halfScreen.y, pos.z, "
    "pos.w);"

    "    TexCoord = aTexCoord;"
    "    exColor = color;"
    "}";

static const char fragShaderSrc[] =
    "#version 450 core\n"
    "out vec4 FragColor;"

    "in vec2 TexCoord;"
    "in vec4 exColor;"

    "layout(binding = 0) uniform sampler2D alphaMap;"

    "void main() {"
    "    float alpha = texture(alphaMap, TexCoord).r;"

    "    FragColor = exColor;"
    "    FragColor.a = exColor.a * alpha;"
    "}";

static GLuint program;
static GLuint vertShader;
static GLuint fragShader;

static GLuint vao;
static GLuint vbo;
static GLuint ibo;

void InitDrawGlyph() {
  vertShader = glCreateShader(GL_VERTEX_SHADER);
  auto src = vertShaderSrc;
  glShaderSource(vertShader, 1, (const GLchar **)&src, 0);
  glCompileShader(vertShader);

  fragShader = glCreateShader(GL_FRAGMENT_SHADER);
  src = fragShaderSrc;
  glShaderSource(fragShader, 1, (const GLchar **)&src, 0);
  glCompileShader(fragShader);

  program = glCreateProgram();
  glAttachShader(program, vertShader);
  glAttachShader(program, fragShader);

  glLinkProgram(program);

  glCreateVertexArrays(1, &vao);
  glBindVertexArray(vao);
  glGenBuffers(1, &vbo);

  // clang-format off
  float vertices[] = { 
    0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f, 0.0f 
  };

  GLubyte indices[] = { 
    0, 1, 2,
    2, 3, 0 
  };
  // clang-format on

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, 20 * sizeof(GLfloat), vertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void *)(3 * sizeof(float)));

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);

  glGenBuffers(1, &ibo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);
}

void CleanUpDrawGlyph() {
  glDeleteBuffers(1, &vbo);
  glDeleteBuffers(1, &ibo);
  glDeleteVertexArrays(1, &vao);

  glDeleteProgram(program);
  glDeleteShader(vertShader);
  glDeleteShader(fragShader);
}

void DrawGlyph(const Glyph &glyph, const float &x, const float &y,
               const glm::vec4 &color, const int &screenWidth,
               const int &screenHeight) {

  glUseProgram(program);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glBindTexture(GL_TEXTURE_2D, glyph.texture);

  glm::mat4 transform =
      glm::translate(glm::identity<glm::mat4>(), glm::vec3(x, y, 0));

  glm::mat4 glyphTransform =
      glm::scale(glm::translate(glm::identity<glm::mat4>(),
                                glm::vec3(glyph.bound.x, glyph.bound.y, 0.0f)),
                 glm::vec3(glyph.bound.w, glyph.bound.h, 1.0f));

  glUniform2f(0, screenWidth, screenHeight);
  glUniformMatrix4fv(1, 1, GL_FALSE, glm::value_ptr(glyphTransform));
  glUniformMatrix4fv(2, 1, GL_FALSE, glm::value_ptr(transform));
  glUniform4f(3, color.r, color.g, color.b, color.a);

  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, (void *)0);
}
