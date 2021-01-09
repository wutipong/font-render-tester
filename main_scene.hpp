#pragma once

#include "scene.hpp"
#include <SDL2/SDL.h>
#include <array>
#include <string>

#include "imgui.h"
#include "imgui-filebrowser/imfilebrowser.h"

class MainScene : public Scene {
public:
  virtual bool Init(const Context &context);
  virtual void Tick(const Context &context);
  virtual void Cleanup(const Context &context);

private:
	std::array<char, 4096> buffer = { 0 };
	SDL_Color color = { 0, 0, 0, 255 };
	int fontSize = 64;
	bool isShape = false;

	std::string path = "<none>";

	int selectedFontIndex = -1;
	std::vector<std::string> fontNames;
	std::vector<std::string> fontPaths;

	std::vector<unsigned char> fontData;

	ImGui::FileBrowser dirChooser{ ImGuiFileBrowserFlags_SelectDirectory };
};
