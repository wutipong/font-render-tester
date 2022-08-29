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

  virtual void DoUI(Context &context){};

  virtual ~Scene()= default;

  static void TickCurrent(Context &context);
  template <class T> static bool ChangeScene(Context &context);

  static std::unique_ptr<Scene> &Current() { return currentScene; }

private:
  static std::unique_ptr<Scene> currentScene;
};

template <class T> bool Scene::ChangeScene(Context &context) {
  auto newScene = std::make_unique<T>();

  if (!newScene->Init(context))
    return false;

  if (currentScene)
    currentScene->Cleanup(context);

  currentScene = std::move(newScene);

  return true;
}

#endif
