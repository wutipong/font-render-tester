#include "main_scene.hpp"

#include <filesystem>
#include <fstream>
#include <streambuf>

#include <imgui.h>

#include <utf8cpp/utf8.h>
#include <utf8cpp/utf8/cpp11.h>

bool MainScene::Init(const Context &context) {

  dirChooser.SetTitle("Browse for font directory");

  return true;
}

void MainScene::Tick(const Context &context) {
  ImGui::Begin("Menu");
  {
    if (ImGui::CollapsingHeader("Font Directory",
                                ImGuiTreeNodeFlags_DefaultOpen)) {
      ImGui::Text(currentDirectory.c_str());
      if (ImGui::Button("Browse")) {
        dirChooser.Open();
      }
    }

    if (ImGui::CollapsingHeader("Parameters", ImGuiTreeNodeFlags_DefaultOpen)) {
      if (ImGui::BeginCombo("Font", (selectedFontIndex == -1) ? "<None>": fontNames[selectedFontIndex].c_str())) {
          for (int i = 0; i < fontNames.size(); i++) {
              if (ImGui::Selectable(fontNames[i].c_str(), i == selectedFontIndex)) {
                  selectedFontIndex = i;
              }
          }

        ImGui::EndCombo();
      }
      ImGui::SliderInt("Font Size", &fontSize, 0, 128);
      ImGui::Checkbox("Shape Text", &isShape);

      float c[3]{color.r / 255.0f, color.g / 255.0f, color.b / 255.0f};

      ImGui::ColorPicker3("color", c, ImGuiColorEditFlags_InputRGB);
      color.r = c[0] * 255;
      color.g = c[1] * 255;
      color.b = c[2] * 255;
      color.a = 255;
    }
  }
  ImGui::End();

  ImGui::Begin("Input Text");
  { ImGui::InputTextMultiline("", buffer.data(), buffer.size()); }
  ImGui::End();

  dirChooser.Display();
  if (dirChooser.HasSelected()) {
      OnDirectorySelected(dirChooser.GetSelected());
      dirChooser.ClearSelected();
  }
}

void MainScene::Cleanup(const Context &context) {}

void MainScene::OnDirectorySelected(const std::filesystem::path& path) {
    currentDirectory = path.string();
    fontPaths = ListFontFiles(currentDirectory);

    fontNames.clear();
    for (auto& p : fontPaths) {
        fontNames.push_back(GetFontName(p));
    }

    selectedFontIndex = -1;
}


std::vector<std::string> MainScene::ListFontFiles(const std::string& path) {
    std::vector<std::string> output;
    for (auto& p : std::filesystem::directory_iterator(path)) {
        if (!p.is_regular_file()) {
            continue;
        }

        auto entryPath = p.path();
        auto extension = entryPath.extension().string();
        if (extension != ".otf" && extension != ".ttf")
            continue;

        output.push_back(p.path().string());
    }

    return output;
}

static std::string ConvertFromFontString(const char* str, const int& length) {
    const char16_t* c16str = reinterpret_cast<const char16_t*>(str);
    std::vector<char16_t> buffer;
    for (int i = 0; i < length/2; i++) {
        buffer.push_back(SDL_SwapBE16(c16str[i]));
    }

    std::string output;
    utf8::utf16to8(buffer.begin(), buffer.end(), std::back_inserter(output));

    return output;
}

std::string MainScene::GetFontName(const std::string& path) {
    stbtt_fontinfo font;
    std::vector<unsigned char> data;

    std::basic_ifstream<unsigned char> file(path, std::ios::binary);
    data =
        std::vector<unsigned char>{ std::istreambuf_iterator<unsigned char>(file),
                                   std::istreambuf_iterator<unsigned char>() };

    file.close();

    if (!stbtt_InitFont(&font,
        reinterpret_cast<unsigned char*>(data.data()), 0)) {
        return "";
    }

    int length;
    auto fontName = ConvertFromFontString(stbtt_GetFontNameString(&font, &length, STBTT_PLATFORM_ID_MICROSOFT, STBTT_MS_EID_UNICODE_BMP, STBTT_MS_LANG_ENGLISH, 1), length);
    auto style = ConvertFromFontString(stbtt_GetFontNameString(&font, &length, STBTT_PLATFORM_ID_MICROSOFT, STBTT_MS_EID_UNICODE_BMP, STBTT_MS_LANG_ENGLISH, 2), length);

    return fontName + " - " + style;
}