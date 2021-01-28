#include "draw_rect.hpp"

#include <GL/gl3w.h>
#include <glm/gtc/type_ptr.hpp>

static const char vertShaderSrc[] =
    "#version 450 core\n"

    "layout(location = 0) in vec2 in_Position;"

    "layout(location = 0) uniform vec2 screen;"
    "layout(location = 1) uniform vec4 in_Color;"
    "layout(location = 2) uniform mat4 transform;"

    "out vec4 ex_Color;"

    "void main(void) {"
    "    vec4 pos = transform * vec4(in_Position.x, in_Position.y, 0.0, 1.0);"

    "    vec2 halfScreen = screen / 2;"
    "    gl_Position = vec4((pos.x - halfScreen.x) / halfScreen.x,"
    "                       (pos.y - halfScreen.y) / halfScreen.y, pos.z, pos.w); "

    "    ex_Color = in_Color;"
    "}";

static const char fragShaderSrc[] = "#version 450 core\n"

                                    "precision highp float;"

                                    "in  vec4 ex_Color;"
                                    "out vec4 gl_FragColor;"

                                    "void main(void) {"
                                    "    gl_FragColor = ex_Color;"
                                    "}";

static GLuint program;
static GLuint vertShader;
static GLuint fragShader;

static GLuint vao;
static GLuint vbo;

void InitDrawRect() {
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
    0, 0,
    0, -1,
    1, -1,
    1, 0,
  };
  // clang-format on

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), vertices, GL_DYNAMIC_DRAW);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
  glEnableVertexAttribArray(0);
}

void CleanUpDrawRect() {
  glDeleteBuffers(1, &vbo);
  glDeleteVertexArrays(1, &vao);

  glDeleteProgram(program);
  glDeleteShader(vertShader);
  glDeleteShader(fragShader);
}

void DrawRect(const float &x, const float &y, const float &w, const float &h,
              const glm::vec4 &color, const int &screenWidth,
              const int &screenHeight) {
  glUseProgram(program);

  glm::mat4 transform =
      glm::scale(glm::translate(glm::identity<glm::mat4>(), glm::vec3(x, y, 0)),
                 glm::vec3(w, h, 1.0f));
  glUniform2f(0, screenWidth, screenHeight);
  glUniform4f(1, color.r, color.g, color.b, color.a);
  glUniformMatrix4fv(2, 1, GL_FALSE, glm::value_ptr(transform));

  glDrawArrays(GL_LINE_LOOP, 0, 4);
}
