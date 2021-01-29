#include "test_scene.hpp"

#include "GL/gl3w.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "imgui.h"

#include "draw_rect.hpp"
#include "draw_glyph.hpp"

#include <chrono>
#include <random>

static std::vector<glm::vec2>
RecreatePositions(const int &count, const int &width, const int &height) {
  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  std::default_random_engine generator(seed);

  std::vector<glm::vec2> output;
  output.resize(count);

  std::uniform_real_distribution<float> xGen(0, width);
  std::uniform_real_distribution<float> yGen(0, height);

  for (auto &v : output) {
    v.x = xGen(generator);
    v.y = yGen(generator);
  }

  return output;
}

bool TestScene::Init(Context &ctx) {
  positions = RecreatePositions(count, ctx.windowBound.w, ctx.windowBound.h);
  
  InitDrawRect();
  InitDrawGlyph();

  font = { "fonts/NotoSans-Regular.ttf" };
  font.SetFontSize(256);

  glyph = font.GetGlyphFromChar('^');

  return true;
};

void TestScene::Tick(Context &ctx) {

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glClearColor(ctx.backgroundColor.r, ctx.backgroundColor.g,
               ctx.backgroundColor.b, ctx.backgroundColor.a);
  glClear(GL_COLOR_BUFFER_BIT);

  for (auto &v : positions) {
      DrawRect(v.x, v.y, w, h, color, ctx.windowBound.w, ctx.windowBound.h);
      DrawGlyph(glyph, v.x, v.y, color, ctx.windowBound.w, ctx.windowBound.h);
  }
};

void TestScene::Cleanup(Context &context) {
    CleanUpDrawGlyph();
    CleanUpDrawRect();
};

void TestScene::DoUI(Context &context) {
  ImGui::Begin("Input");
  {
    if (ImGui::SliderInt("Count", &count, 1, 1000)) {
      positions = RecreatePositions(count, context.windowBound.w,
                                    context.windowBound.h);
    }
    ImGui::ColorEdit4("Color", glm::value_ptr(color),
                      ImGuiColorEditFlags_Float | ImGuiColorEditFlags_AlphaBar);

    ImGui::SliderFloat("Width", &w, 0, 2000);
    ImGui::SliderFloat("Height", &h, 0, 2000);
  }
  ImGui::End();
};
