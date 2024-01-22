#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include <filesystem>

struct Settings {
  std::filesystem::path fontPath{std::filesystem::absolute("fonts").string()};
};

void SaveSettings(const Settings &settings);
Settings LoadSettings();

#endif