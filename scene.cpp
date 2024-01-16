#include "scene.hpp"

std::unique_ptr<Scene> Scene::currentScene;

void Scene::TickCurrent(SDL_Renderer *renderer, Context &context) {
  if (currentScene == nullptr)
    return;
  currentScene->Tick(renderer, context);
}
