#include "context.hpp"

#include <fstream>
#include <nlohmann/json.hpp>

using namespace nlohmann;

void SaveContext(const Context& ctx, const std::filesystem::path& path) {
	json js;
	
	js["window_bound"]["width"] = ctx.windowBound.w;
	js["window_bound"]["height"] = ctx.windowBound.h;
	
	std::string str = js.dump();
	std::fstream output(path, std::ios::out);

	output.write(str.c_str(), str.length());

	output.close();
	
}

void LoadContext(Context& ctx, const std::filesystem::path& path) {
	if (!std::filesystem::exists(path)) {
		return;
	}

	std::fstream input(path, std::ios::in);
	std::string str = { std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>() };
	input.close();

	json js = json::parse(str);

	ctx.windowBound.w = js["window_bound"]["width"];
	ctx.windowBound.h = js["window_bound"]["height"];
}
