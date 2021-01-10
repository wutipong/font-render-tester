#pragma once

#include "scene.hpp"
#include <SDL2/SDL.h>
#include <stb_truetype.h>
#include <array>
#include <string>
#include <filesystem>

#include "imgui.h"
#include "imgui-filebrowser/imfilebrowser.h"

#include "font.hpp"

class MainScene : public Scene {
public:
  virtual bool Init(const Context &context);
  virtual void Tick(const Context &context);
  virtual void Cleanup(const Context &context);

  void OnDirectorySelected(const std::filesystem::path& path);

  static std::vector<std::filesystem::path> ListFontFiles(const std::string& path);

private:
	std::array<char, 4096> buffer = { 0 };
	SDL_Color color = { 0, 0, 0, 255 };
	int fontSize = 64;
	bool isShape = false;

	std::string currentDirectory = "<none>";

	int selectedFontIndex = -1;
	std::vector<std::filesystem::path> fontPaths;

	std::vector<unsigned char> fontData;

	ImGui::FileBrowser dirChooser{ ImGuiFileBrowserFlags_SelectDirectory };

	Font font{};
};
