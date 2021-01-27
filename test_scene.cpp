#include "test_scene.hpp"

#include "GL/gl3w.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "imgui.h"

bool TestScene::Init(Context &ctx) { return true; };
void TestScene::Tick(Context &ctx) {
  glClearColor(ctx.backgroundColor.r, ctx.backgroundColor.g,
               ctx.backgroundColor.b, ctx.backgroundColor.a);
  glClear(GL_COLOR_BUFFER_BIT);
};
void TestScene::Cleanup(Context &context){};
void TestScene::DoUI(Context &context) {
  ImGui::Begin("Input Text");
  {
    ImGui::ColorEdit4("line color", glm::value_ptr(lineColor),
                      ImGuiColorEditFlags_Float | ImGuiColorEditFlags_AlphaBar);
    ImGui::InputFloat4("Point 1", glm::value_ptr(point1), 4);
    ImGui::InputFloat4("Point 2", glm::value_ptr(point2), 4);
  }
  ImGui::End();
};
