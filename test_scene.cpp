#include "test_scene.hpp"

#include "GL/gl3w.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "imgui.h"

static const std::string vertShaderSrc =
    "#version 150\n"

    "in  vec2 in_Position;"
    "in  vec3 in_Color;"

    "out vec3 ex_Color;"

    "void main(void) {"
    "    gl_Position = vec4(in_Position.x, in_Position.y, 0.0, 1.0);"

    "    ex_Color = in_Color;"
    "}";

static const std::string fragShaderSrc =
    "#version 150\n"

    "precision highp float;"

    "in  vec3 ex_Color;"
    "out vec4 gl_FragColor;"

    "void main(void) {"
    "    gl_FragColor = vec4(ex_Color, 1.0);"
    "}";

bool TestScene::Init(Context &ctx) {
  vertShader = glCreateShader(GL_VERTEX_SHADER);
  auto src = vertShaderSrc.c_str();
  glShaderSource(vertShader, 1, (const GLchar **)&src, 0);
  glCompileShader(vertShader);

  fragShader = glCreateShader(GL_FRAGMENT_SHADER);
  src = fragShaderSrc.c_str();
  glShaderSource(fragShader, 1, (const GLchar **)&src, 0);
  glCompileShader(fragShader);

  program = glCreateProgram();
  glAttachShader(program, vertShader);
  glAttachShader(program, fragShader);

  return true;
};

void TestScene::Tick(Context &ctx) {
  glClearColor(ctx.backgroundColor.r, ctx.backgroundColor.g,
               ctx.backgroundColor.b, ctx.backgroundColor.a);
  glClear(GL_COLOR_BUFFER_BIT);
};
void TestScene::Cleanup(Context &context){};
void TestScene::DoUI(Context &context) {
  ImGui::Begin("Input");
  {
    ImGui::InputFloat4("Point 1", glm::value_ptr(point1), 2);
    ImGui::ColorEdit4("Color 1", glm::value_ptr(color1),
                      ImGuiColorEditFlags_Float | ImGuiColorEditFlags_AlphaBar);
    ImGui::InputFloat4("Point 2", glm::value_ptr(point2), 2);
    ImGui::ColorEdit4("Color 2", glm::value_ptr(color2),
                      ImGuiColorEditFlags_Float | ImGuiColorEditFlags_AlphaBar);
  }
  ImGui::End();
};
