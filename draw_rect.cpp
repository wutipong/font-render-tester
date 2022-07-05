#include "draw_rect.hpp"

#include "gl_util.hpp"
#include "io_util.hpp"
#include <GL/gl3w.h>
#include <glm/gtc/type_ptr.hpp>

static GLuint program;
static GLuint vertShader;
static GLuint fragShader;

static GLuint vao;
static GLuint vbo;

void InitDrawRect() {
  vertShader = CompileShader("shaders/draw_rect.vert", "Draw Rect Vertex",
                             GL_VERTEX_SHADER);
  fragShader = CompileShader("shaders/draw_rect.frag", "Draw Rect Fragment",
                             GL_FRAGMENT_SHADER);

  program = glCreateProgram();
  glAttachShader(program, vertShader);
  glAttachShader(program, fragShader);

  glLinkProgram(program);
  SetGLObjectLabel(GL_PROGRAM, program, "Draw Rect Program");

  glCreateVertexArrays(1, &vao);
  SetGLObjectLabel(GL_VERTEX_ARRAY, vao, "Draw Rect VAO");
  glBindVertexArray(vao);

  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  SetGLObjectLabel(GL_BUFFER, vbo, "Draw Rect VBO");

  // clang-format off
  float vertices[] = {
    0, 0,
    0, -1,
    1, -1,
    1, 0,
  };
  // clang-format on
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
              const int &screenHeight, const DrawRectMode &mode) {
  glUseProgram(program);

  glm::mat4 transform =
      glm::scale(glm::translate(glm::identity<glm::mat4>(), glm::vec3(x, y, 0)),
                 glm::vec3(w, h, 1.0f));
  glUniform2f(0, screenWidth, screenHeight);
  glUniform4f(1, color.r, color.g, color.b, color.a);
  glUniformMatrix4fv(2, 1, GL_FALSE, glm::value_ptr(transform));

  GLenum drawMode = GL_LINE_LOOP;
  switch (mode) {
  case DrawRectMode::Fill:
    drawMode = GL_TRIANGLE_FAN;
    break;

  case DrawRectMode::Outline:
    drawMode = GL_LINE_LOOP;
    break;
  }
  glDrawArrays(drawMode, 0, 4);
}
