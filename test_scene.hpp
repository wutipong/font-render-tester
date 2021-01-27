#pragma once

#include "scene.hpp"

#include "glm/glm.hpp"

class TestScene : public Scene {
public:
  virtual bool Init(Context &context) override;
  virtual void Tick(Context &context) override;
  virtual void Cleanup(Context &context) override;
  virtual void DoUI(Context &context) override;

private:
  glm::vec4 color1 = glm::vec4{1, 1, 1, 1};
  glm::vec4 point1 = glm::vec4{0, 0, 1.0f, 1.0f};
  glm::vec4 color2 = glm::vec4{1, 1, 1, 1};
  glm::vec4 point2 = glm::vec4{WIDTH, HEIGHT, 1.0f, 1.0f};
};
