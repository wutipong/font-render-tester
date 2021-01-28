#pragma once

#include "glm/glm.hpp"
#include "scene.hpp"
#include "font.hpp"
#include <vector>

class TestScene : public Scene {
public:
  virtual bool Init(Context &context) override;
  virtual void Tick(Context &context) override;
  virtual void Cleanup(Context &context) override;
  virtual void DoUI(Context &context) override;

private:
  glm::vec4 color = glm::vec4{1, 0, 1, 1};
  float w = 200;
  float h = 300;

  int count = 10;

  std::vector<glm::vec2> positions;

  Font font;
  Glyph glyph;
};
