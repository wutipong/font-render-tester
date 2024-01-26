#ifndef MAIN_SCENE_HPP
#define MAIN_SCENE_HPP

#include "context.hpp"
#include <SDL2/SDL.h>

bool SceneInit(Context &context);
void SceneTick(SDL_Renderer *renderer, Context &ctx);
void SceneCleanUp(Context &context);
void SceneDoUI(Context &context);

#endif