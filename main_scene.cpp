#include "main_scene.hpp"

#include <filesystem>
#include <fstream>
#include <streambuf>

#include <imgui.h>

#include <utf8cpp/utf8.h>
#include <utf8cpp/utf8/cpp11.h>

#include "text_renderer.hpp"

bool MainScene::Init(Context &context) {
  dirChooser.SetTitle("Browse for font directory");
  OnDirectorySelected(context, context.fontPath);
  dirChooser.SetPwd(context.fontPath);

  return true;
}

void MainScene::Tick(Context &context) {
  int newSelected = selectedFontIndex;
  ImGui::Begin("Menu");
  {
    if (ImGui::CollapsingHeader("Font Directory",
                                ImGuiTreeNodeFlags_DefaultOpen)) {
      ImGui::Text(context.fontPath.c_str());
      if (ImGui::Button("Browse")) {
        dirChooser.Open();
      }
    }

    if (ImGui::CollapsingHeader("Font", ImGuiTreeNodeFlags_DefaultOpen)) {
      if (ImGui::BeginCombo(
              "Font File",
              (selectedFontIndex == -1)
                  ? "<None>"
                  : fontPaths[selectedFontIndex].filename().string().c_str())) {
        for (int i = 0; i < fontPaths.size(); i++) {
          if (ImGui::Selectable(fontPaths[i].filename().string().c_str(),
                                i == selectedFontIndex)) {
            newSelected = i;
          }
        }

        ImGui::EndCombo();
      }

      auto familyName = font.GetFamilyName();
      auto subFamily = font.GetSubFamilyName();
      ImGui::InputText("Family Name", const_cast<char *>(familyName.c_str()),
                       familyName.size(), ImGuiInputTextFlags_ReadOnly);
      ImGui::InputText("Sub Family Name", const_cast<char *>(subFamily.c_str()),
                       subFamily.size(), ImGuiInputTextFlags_ReadOnly);
    }

    if (ImGui::CollapsingHeader("Parameters", ImGuiTreeNodeFlags_DefaultOpen)) {
      ImGui::SliderInt("Font Size", &fontSize, 0, 128);
      ImGui::Checkbox("Shape Text", &isShape);

      if (isShape) {
        if (ImGui::BeginCombo("Script", scriptStrs[selectedScript])) {
          for (int i = 0; i < scriptStrs.size(); i++) {
            if (ImGui::Selectable(scriptStrs[i], i == selectedScript)) {
              selectedScript = i;
            }
          }
          ImGui::EndCombo();
        }
        if (ImGui::BeginCombo("Direction", directionStrs[selectedDirection])) {
          for (int i = 0; i < directionStrs.size(); i++) {
            if (ImGui::Selectable(directionStrs[i], i == selectedDirection)) {
              selectedDirection = i;
            }
          }
          ImGui::EndCombo();
        }
      }

      float c[3]{color.r / 255.0f, color.g / 255.0f, color.b / 255.0f};

      ImGui::ColorPicker3("color", c, ImGuiColorEditFlags_InputRGB);
      color.r = c[0] * 255;
      color.g = c[1] * 255;
      color.b = c[2] * 255;
      color.a = 255;

      ImGui::Checkbox("Debug", &context.debug);
    }
  }
  ImGui::End();

  ImGui::Begin("Input Text");
  { ImGui::InputTextMultiline("", buffer.data(), buffer.size()); }
  ImGui::End();

  dirChooser.Display();
  if (dirChooser.HasSelected()) {
    OnDirectorySelected(context, dirChooser.GetSelected());
    dirChooser.ClearSelected();
    newSelected = -1;
  }

  if (newSelected != selectedFontIndex) {
    selectedFontIndex = newSelected;
    if (selectedFontIndex == -1) {
      font = Font();
    } else {
      font = Font(fontPaths[selectedFontIndex].string());
    }
  }

  auto textRender = TextRenderEnum::NoShape;
  if (isShape) {
    switch (directions[selectedDirection]) {
    case HB_DIRECTION_LTR:
      textRender = TextRenderEnum::LeftToRight;
      break;
    case HB_DIRECTION_TTB:
      textRender = TextRenderEnum::TopToBottom;
      break;
    case HB_DIRECTION_RTL:
      textRender = TextRenderEnum::RightToLeft;
      break;
    }
  }
  font.SetTextRenderer(textRender);

  font.SetFontSize(fontSize);

  font.RenderText(context, std::string(buffer.data()), color,
                  scripts[selectedScript]);
}

void MainScene::Cleanup(Context &context) {}

void MainScene::OnDirectorySelected(Context& ctx, const std::filesystem::path &path) {
  std::filesystem::path newPath = path;

  if (!std::filesystem::exists(newPath)) {
    newPath = std::filesystem::absolute("fonts");
  }
  ctx.fontPath = path.string();
  fontPaths = ListFontFiles(ctx.fontPath);
}

std::vector<std::filesystem::path>
MainScene::ListFontFiles(const std::filesystem::path &path) {
  std::vector<std::filesystem::path> output;
  for (auto &p : std::filesystem::directory_iterator(path)) {
    if (!p.is_regular_file()) {
      continue;
    }

    auto entryPath = p.path();
    auto extension = entryPath.extension().string();
    if (extension != ".otf" && extension != ".ttf")
      continue;

    output.push_back(p);
  }

  return output;
}
