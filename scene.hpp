#ifndef SCENE_HPP
#define SCENE_HPP

#include <SDL2/SDL.h>
#include <memory>

#include "context.hpp"

class Scene {
public:
  virtual bool Init(Context &context) = 0;
  virtual void Tick(Context &context) = 0;
  virtual void Cleanup(Context &context) = 0;

  static void TickCurrent(Context &context);
  template <class T> static void ChangeScene(Context &context);

private:
  static std::unique_ptr<Scene> currentScene;
};

template <class T> void Scene::ChangeScene(Context &context) {
  auto newScene = std::make_unique<T>();

  if (!newScene->Init(context))
    return;

  currentScene->Cleanup(context);
  currentScene = std::move(newScene);
}

#endif
