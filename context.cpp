#include "context.hpp"

#include "io_util.hpp"
#include <fstream>
#include <nlohmann/json.hpp>

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

  // If there's something wrong with reading file, just do nothing and return.
  try {
    std::string str;
    LoadFile(path.string(), str);

    js = json::parse(str);
  } catch (...) {
    return;
  }

  // FIXME: using try/catch just to ignore exceptions is not really a good
  // practice.
  try {
    ctx.windowBound.w = js["window_bound"]["width"];
  } catch (...) {
  }
  try {
    ctx.windowBound.h = js["window_bound"]["height"];
  } catch (...) {
  }
  try {
    ctx.fontPath = js["font_path"];
  } catch (...) {
  }
}
