#include "context.hpp"

#include "io_util.hpp"
#include <fstream>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

using namespace nlohmann;

void SaveContext(const Context &ctx, const std::filesystem::path &path) {
  json js;

  js["window_bound"]["width"] = ctx.windowBound.w;
  js["window_bound"]["height"] = ctx.windowBound.h;

  js["font_path"] = ctx.fontPath;

  std::string str = js.dump();
  std::fstream output(path, std::ios::out);

  output.write(str.c_str(), str.length());

  output.close();
}

void LoadContext(Context &ctx, const std::filesystem::path &path) {
  if (!std::filesystem::exists(path)) {
    return;
  }

  json js;

  // If there's something wrong with reading file, log an error and return. 
  try {
    std::string str;
    str = LoadFile<std::string>(path);

    js = json::parse(str);
  } catch (const json::exception& e) {
    spdlog::error("Error reading context file: {}", e.what());
    return;
  }

  Context c = ctx;

  try {
    c.windowBound.w = js["window_bound"]["width"];
    c.windowBound.h = js["window_bound"]["height"];
    c.fontPath = js["font_path"];
  } catch (const json::exception& e) {
    spdlog::error("Error reading context file value: {}", e.what());
  }

  ctx = c;
}
