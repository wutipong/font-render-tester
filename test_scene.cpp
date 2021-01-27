#include "test_scene.hpp"

#include "GL/gl3w.h"

bool TestScene::Init(Context &ctx) { return true; };
void TestScene::Tick(Context &ctx) {
  glClearColor(ctx.backgroundColor.r, ctx.backgroundColor.g,
               ctx.backgroundColor.b, ctx.backgroundColor.a);
  glClear(GL_COLOR_BUFFER_BIT);
};
void TestScene::Cleanup(Context &context){};
void TestScene::DoUI(Context &context){};
