#include "main_scene.hpp"

#include "colors.hpp"
#include "text_renderer.hpp"
#include <algorithm>
#include <filesystem>
#include <imgui.h>


namespace {
constexpr ImVec4 SDLColorToImVec4(const SDL_Color &color) {
  ImVec4 output{
      static_cast<float>(color.r) / 255.0f,
      static_cast<float>(color.g) / 255.0f,
      static_cast<float>(color.b) / 255.0f,
      static_cast<float>(color.a) / 255.0f,
  };

  return output;
}

constexpr auto f4DebugGlyphBoundColor = SDLColorToImVec4(debugGlyphBoundColor);
constexpr auto f4DebugAscendColor = SDLColorToImVec4(debugAscendColor);
constexpr auto f4DebugDescendColor = SDLColorToImVec4(debugDescendColor);
constexpr auto f4DebugBaselineColor = SDLColorToImVec4(debugBaselineColor);
constexpr auto f4DebugCaretColor = SDLColorToImVec4(debugCaretColor);

} // namespace

bool MainScene::Init(Context &context) {
  if (!Font::Init())
    return false;

  dirChooser.SetTitle("Browse for font directory");
  OnDirectorySelected(context, context.fontPath);
  dirChooser.SetPwd(context.fontPath);

  return true;
}

void MainScene::Tick(SDL_Renderer *renderer, Context &ctx) {

  SDL_SetRenderDrawColor(renderer, backgroundColor.r, backgroundColor.g,
                         backgroundColor.b, backgroundColor.a);
  SDL_RenderClear(renderer);

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

  SDL_Color sdlColor = {
      static_cast<uint8_t>(color[0] * 255.0f),
      static_cast<uint8_t>(color[1] * 255.0f),
      static_cast<uint8_t>(color[2] * 255.0f),
      0xFF,
  };

  font.RenderText(renderer, ctx, std::string(buffer.data()), sdlColor,
                  languages[selectedLanguage].code,
                  scripts[selectedScript].script);
}

void MainScene::Cleanup(Context &context) { Font::CleanUp(); }

void MainScene::DoUI(Context &context) {
  int newSelected = selectedFontIndex;

  if (ImGui::Begin("Menu")) {
    if (ImGui::CollapsingHeader("Font Directory")) {
      ImGui::Text(context.fontPath.c_str());
      if (ImGui::Button("Browse")) {
        dirChooser.Open();
      }
      if (ImGui::Button("Re-scan")) {
        fontPaths = ListFontFiles(context.fontPath);
      }
    }

    if (ImGui::CollapsingHeader("Font", ImGuiTreeNodeFlags_DefaultOpen)) {
      auto currentFile = selectedFontIndex == -1
                             ? "<None>"
                             : fontPaths[selectedFontIndex].filename().string();

      if (ImGui::BeginCombo("Font File", currentFile.c_str())) {
        for (int i = 0; i < fontPaths.size(); i++) {
          auto isSelected = i == selectedFontIndex;
          if (isSelected) {
            ImGui::SetItemDefaultFocus();
          }

          if (ImGui::Selectable(fontPaths[i].filename().string().c_str(),
                                isSelected)) {
            newSelected = i;
          }
        }

        ImGui::EndCombo();
      }

      auto familyName = font.GetFamilyName();
      auto subFamily = font.GetSubFamilyName();

      ImGui::InputText("Family Name", familyName.data(), familyName.size(),
                       ImGuiInputTextFlags_ReadOnly);

      ImGui::InputText("Sub Family Name", subFamily.data(), subFamily.size(),
                       ImGuiInputTextFlags_ReadOnly);
    }

    if (ImGui::CollapsingHeader("Parameters", ImGuiTreeNodeFlags_DefaultOpen)) {
      ImGui::SliderInt("Font Size", &fontSize, 0, 128);
      if (ImGui::CollapsingHeader("Font Metrics", ImGuiTreeNodeFlags_None)) {
        auto ascend = font.Ascend();
        auto descend = font.Descend();
        auto lineGap = font.LineGap();
        auto lineHeight = font.LineHeight();

        ImGui::InputFloat("Ascend", &ascend, ImGuiInputTextFlags_ReadOnly);
        ImGui::InputFloat("Descend", &descend, ImGuiInputTextFlags_ReadOnly);
        ImGui::InputFloat("Line Gap", &lineGap, ImGuiInputTextFlags_ReadOnly);
        ImGui::InputFloat("Line Height", &lineHeight,
                          ImGuiInputTextFlags_ReadOnly);
      }
      ImGui::Checkbox("Shape Text", &isShape);

      if (isShape) {
        if (ImGui::BeginCombo("Language", languages[selectedLanguage].name)) {
          for (size_t i = 0; i < languages.size(); i++) {
            if (ImGui::Selectable(languages[i].name, i == selectedLanguage)) {
              selectedLanguage = i;
            }
          }
          ImGui::EndCombo();
        }
        if (ImGui::BeginCombo("Script", scripts[selectedScript].name)) {
          for (size_t i = 0; i < scripts.size(); i++) {
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

      ImGui::ColorPicker3("color", color, ImGuiColorEditFlags_InputRGB);
    }
  }
  ImGui::End();

  if (ImGui::Begin("Input Text")) {
    ImGui::InputTextMultiline("##InputText", buffer.data(), buffer.size());
  }
  ImGui::End();

  if (ImGui::Begin("Debug")) {
    ImGui::Checkbox("Enabled", &context.debug);
    if (context.debug) {
      if (ImGui::CollapsingHeader("Features")) {
        ImGui::Checkbox("Baseline", &context.debugBaseline);
        ImGui::SameLine();
        ImGui::ColorButton(
            "Baseline", f4DebugBaselineColor,
            ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoPicker |
                ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoLabel);

        ImGui::Checkbox("Caret Positions", &context.debugCaret);
        ImGui::SameLine();
        ImGui::ColorButton(
            "Caret Positions", f4DebugCaretColor,
            ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoPicker |
                ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoLabel);

        ImGui::Checkbox("Glyph Bound", &context.debugGlyphBound);
        ImGui::SameLine();
        ImGui::ColorButton(
            "Glyph Bound", f4DebugGlyphBoundColor,
            ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoPicker |
                ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoLabel);

        ImGui::Checkbox("Ascend", &context.debugAscend);
        ImGui::SameLine();
        ImGui::ColorButton(
            "Ascend", f4DebugAscendColor,
            ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoPicker |
                ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoLabel);

        ImGui::Checkbox("Descend", &context.debugDescend);
        ImGui::SameLine();
        ImGui::ColorButton(
            "Descend", f4DebugDescendColor,
            ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoPicker |
                ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoLabel);
      }
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

    static auto compare = [](char c1, char c2) -> bool {
      return std::tolower(c1) == std::tolower(c2);
    };

    if (!std::equal(extension.begin(), extension.end(), ".otf", compare) &&
        !std::equal(extension.begin(), extension.end(), ".ttf", compare)) {
      continue;
    }

    output.push_back(p);
  }

  return output;
}
