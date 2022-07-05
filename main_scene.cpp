#include "main_scene.hpp"

#include "text_renderer.hpp"
#include <filesystem>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>

bool MainScene::Init(Context &context) {

  Font::Init();
  InitTextRenderers();
  dirChooser.SetTitle("Browse for font directory");
  OnDirectorySelected(context, context.fontPath);
  dirChooser.SetPwd(context.fontPath);

  return true;
}

void MainScene::Tick(Context &context) {

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glClearColor(context.backgroundColor.r, context.backgroundColor.g,
               context.backgroundColor.b, context.backgroundColor.a);
  glClear(GL_COLOR_BUFFER_BIT);
  auto textRender = TextRenderEnum::NoShape;
  if (isShape) {
    switch (directions[selectedDirection].direction) {
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
                  languages[selectedLanguage].code,
                  scripts[selectedScript].script);
}

void MainScene::Cleanup(Context &context) {
  CleanUpTextRenderers();
  Font::CleanUp();
}

void MainScene::DoUI(Context &context) {
  int newSelected = selectedFontIndex;

  ImGui::Begin("Menu");
  {
    if (ImGui::CollapsingHeader("Font Directory")) {
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
        if (ImGui::BeginCombo("Language", languages[selectedLanguage].name)) {
          for (int i = 0; i < languages.size(); i++) {
            if (ImGui::Selectable(languages[i].name, i == selectedLanguage)) {
              selectedLanguage = i;
            }
          }
          ImGui::EndCombo();
        }
        if (ImGui::BeginCombo("Script", scripts[selectedScript].name)) {
          for (int i = 0; i < scripts.size(); i++) {
            if (ImGui::Selectable(scripts[i].name, i == selectedScript)) {
              selectedScript = i;
            }
          }
          ImGui::EndCombo();
        }
        if (ImGui::BeginCombo("Direction",
                              directions[selectedDirection].name)) {
          for (int i = 0; i < directions.size(); i++) {
            if (ImGui::Selectable(directions[i].name, i == selectedDirection)) {
              selectedDirection = i;
            }
          }
          ImGui::EndCombo();
        }
      }

      ImGui::ColorPicker3("color", glm::value_ptr(color),
                          ImGuiColorEditFlags_InputRGB);
    }
  }
  ImGui::End();

  ImGui::Begin("Input Text");
  { ImGui::InputTextMultiline("##InputText", buffer.data(), buffer.size()); }
  ImGui::End();

  ImGui::Begin("Debug");
  {
    ImGui::Checkbox("Enabled", &context.debug);
    if (context.debug) {
      ImGui::ColorEdit4(
          "Glyph Bound", glm::value_ptr(context.debugGlyphBoundColor),
          ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoPicker |
              ImGuiColorEditFlags_NoTooltip);
      ImGui::ColorEdit4("Ascend", glm::value_ptr(context.debugAscendColor),
                        ImGuiColorEditFlags_NoInputs |
                            ImGuiColorEditFlags_NoPicker |
                            ImGuiColorEditFlags_NoTooltip);
      ImGui::ColorEdit4("Descend", glm::value_ptr(context.debugDescendColor),
                        ImGuiColorEditFlags_NoInputs |
                            ImGuiColorEditFlags_NoPicker |
                            ImGuiColorEditFlags_NoTooltip);
      ImGui::ColorEdit4("Baseline", glm::value_ptr(context.debugLineColor),
                        ImGuiColorEditFlags_NoInputs |
                            ImGuiColorEditFlags_NoPicker |
                            ImGuiColorEditFlags_NoTooltip);
    }
  }
  ImGui::End();
  dirChooser.Display();
  if (dirChooser.HasSelected()) {
    OnDirectorySelected(context, dirChooser.GetSelected());
    dirChooser.ClearSelected();
    newSelected = -1;
  }

  if (newSelected != selectedFontIndex) {
    if (newSelected == -1) {
      font = Font();
      selectedFontIndex = newSelected;
    } else {
      Font newFont;
      if (!newFont.LoadFile(fontPaths[newSelected].string())) {
        ImGui::OpenPopup("InvalidFont");
      } else {
        font = newFont;
        selectedFontIndex = newSelected;
      }
    }
  }

  if (ImGui::BeginPopup("InvalidFont")) {
    ImGui::Text("Invalid Font File");
    if (ImGui::Button("Close")) {
      ImGui::CloseCurrentPopup();
    }
    ImGui::EndPopup();
  }
}

void MainScene::OnDirectorySelected(Context &ctx,
                                    const std::filesystem::path &path) {
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
