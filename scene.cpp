#include "scene.hpp"

std::unique_ptr<Scene> Scene::currentScene;

void Scene::TickCurrent(Context &context, SDL_Renderer* renderer) {
  if (currentScene == nullptr)
    return;
  currentScene->Tick(context, renderer);
}
