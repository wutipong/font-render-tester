#include "settings.hpp"
#include "io_util.hpp"
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

using namespace nlohmann;

namespace {
static constexpr char contextJson[] = "settings.json";
}

void SaveSettings(const Settings &settings) {
  json js{};

  js["font_path"] = settings.fontPath;

  std::string str = js.dump();

  const auto preferencePath = GetPreferencePath();
  const auto path = preferencePath / contextJson;

  std::fstream output(path, std::ios::out);

  output.write(str.c_str(), str.length());
  output.close();
}

Settings LoadSettings() {
  const auto preferencePath = GetPreferencePath();
  const auto path = preferencePath / contextJson;

  if (!std::filesystem::exists(path)) {
    return Settings{};
  }

  json js{};

  // If there's something wrong with reading file, log an error and return.
  try {
    std::string str;
    str = LoadFile<std::string>(path);
    js = json::parse(str);
  } catch (const json::exception &e) {
    spdlog::error("Error reading context file: {}", e.what());
    return Settings{};
  }

  try {
    Settings output{};
    output.fontPath = std::string(js["font_path"]);

    return output;
  } catch (const json::exception &e) {
    spdlog::error("Error reading context file value: {}", e.what());
    return Settings{};
  }
}