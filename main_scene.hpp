#pragma once

#include <filesystem>

#include "context.hpp"

namespace MainScene {
bool Init(Context &context);
void Tick(Context &ctx);
void Cleanup(Context &context);
void DoUI(Context &context);

void OnDirectorySelected(Context &context, const std::filesystem::path &path);

static std::vector<std::filesystem::path>
ListFontFiles(const std::filesystem::path &path);
}; // namespace MainScene
