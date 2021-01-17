#include "scene.hpp"

#include "main_scene.hpp"

std::unique_ptr<Scene> Scene::currentScene = std::make_unique<MainScene>();

void Scene::TickCurrent(Context &context) {
  if (currentScene == nullptr)
    return;
  currentScene->Tick(context);
}
