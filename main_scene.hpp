#pragma once

#include <filesystem>

#include "context.hpp"

namespace MainScene {
bool Init(Context &context);
void Tick(Context &ctx);
void Cleanup(Context &context);
void DoUI(Context &context);
}; // namespace MainScene
