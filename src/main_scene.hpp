#ifndef MAIN_SCENE_HPP
#define MAIN_SCENE_HPP

#include "debug_settings.hpp"
#include <SDL3/SDL.h>

bool SceneInit();
void SceneTick(SDL_Renderer *renderer);
void SceneCleanUp();
void SceneDoUI();

#endif