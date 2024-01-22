#include "main_scene.hpp"

#include "colors.hpp"
#include "io_util.hpp"
#include "settings.hpp"
#include "text_renderer.hpp"
#include <algorithm>
#include <array>
#include <filesystem>
#include <fstream>
#include <imgui.h>
#include <imgui_internal.h>
#include <magic_enum_all.hpp>
#include <magic_enum_containers.hpp>
#include <spdlog/spdlog.h>

namespace {
constexpr int toolbarWidth = 400;
constexpr int padding = 30;
} // namespace

bool MainScene::Init(Context &context) {
  if (!Font::Init())
    return false;

  auto [fontPath] = LoadSettings();
  fontDirPath = fontPath.string();

  dirChooser.SetTitle("Browse for font directory");
  OnDirectorySelected(context, fontDirPath);
  dirChooser.SetPwd(fontDirPath);

  std::copy(std::cbegin(exampleText), std::cend(exampleText), buffer.begin());

  return true;
}

void MainScene::Tick(SDL_Renderer *renderer, Context &ctx) {
  SDL_Rect viewport = ctx.windowBound;

  viewport.x += padding;
  viewport.y += padding;
  viewport.w -= (toolbarWidth + padding * 2);
  viewport.h -= 2 * padding;

  SDL_RenderSetViewport(renderer, &viewport);

  SDL_SetRenderDrawColor(renderer, backgroundColor.r, backgroundColor.g,
                         backgroundColor.b, backgroundColor.a);
  SDL_RenderClear(renderer);

  font.SetFontSize(fontSize);

  RenderText(renderer, ctx);

  SDL_RenderGetViewport(renderer, nullptr);
}

void MainScene::CleanUp(Context &context) {
  Font::CleanUp();
  SaveSettings({.fontPath = fontDirPath});
}

void MainScene::DoUI(Context &context) {
  int newSelected = selectedFontIndex;
  if (ImGui::BeginMainMenuBar()) {

    if (ImGui::BeginMenu("File##menu")) {
      if (ImGui::MenuItem("Change font directory##file-menu")) {
        dirChooser.Open();
      }

      if (ImGui::MenuItem("Re-scan font directory##file-menu")) {
        fontFilePaths = ListFontFiles(fontDirPath);
      }

      ImGui::Separator();

      if (ImGui::MenuItem("Exit", "Alt+F4")) {
        SDL_Event ev{
            .quit{
                .type = SDL_QUIT,
                .timestamp = SDL_GetTicks(),
            },
        };
        SDL_PushEvent(&ev);
      }

      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("View##menu")) {
      ImGui::MenuItem("Text editor##view-menu", "", &context.showTextEditor);
      ImGui::MenuItem("Debug##view-menu", "", &context.debug);

      ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
  }

  if (ImGui::BeginViewportSideBar(
          "status bar", nullptr, ImGuiDir_Down, ImGui::GetFrameHeight(),
          ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings |
              ImGuiWindowFlags_MenuBar)) {
    if (ImGui::BeginMenuBar()) {
      ImGui::LabelText("Font Directory", fontDirPath.c_str());
      ImGui::EndMenuBar();
    }
  }
  ImGui::End();

  if (ImGui::BeginViewportSideBar("toolbar", nullptr, ImGuiDir_Right,
                                  toolbarWidth,
                                  ImGuiWindowFlags_NoSavedSettings)) {

    if (ImGui::CollapsingHeader("Font", ImGuiTreeNodeFlags_DefaultOpen)) {
      constexpr int noFontSelected = -1;
      auto currentFile =
          selectedFontIndex == noFontSelected
              ? "<None>"
              : fontFilePaths[selectedFontIndex].filename().string();

      if (ImGui::BeginCombo("Font file", currentFile.c_str())) {
        for (int i = 0; i < fontFilePaths.size(); i++) {
          auto isSelected = i == selectedFontIndex;
          if (isSelected) {
            ImGui::SetItemDefaultFocus();
          }

          if (ImGui::Selectable(fontFilePaths[i].filename().string().c_str(),
                                isSelected)) {
            newSelected = i;
          }
        }

        ImGui::EndCombo();
      }

      ImGui::LabelText("Family name", font.GetFamilyName().c_str());
      ImGui::LabelText("Sub-family name", font.GetSubFamilyName().c_str());
    }

    if (ImGui::CollapsingHeader("Parameters", ImGuiTreeNodeFlags_DefaultOpen)) {
      ImGui::SliderInt("Font Size", &fontSize, 0, 128);

      ImGui::SeparatorText("Variations");
      ImGui::BeginDisabled(!font.IsVariableFont());

      bool axisChanged = false;

      constexpr magic_enum::containers::array<VariationAxis, const char *>
          axisLabel = {
              "Italic##axis", "Optical size##axis", "Slant##axis",
              "Weight##axis", "Width##axis",
          };

      magic_enum::enum_for_each<VariationAxis>(
          [this, &axisChanged, &axisLabel](const VariationAxis &axis) {
            if (axisLimits[axis].has_value()) {
              [[maybe_unused]] auto [min, max, _] = *axisLimits[axis];
              axisChanged |= ImGui::DragFloat(axisLabel[axis], &axisValue[axis],
                                              1.0f, min, max);
            } else {
              ImGui::LabelText(axisLabel[axis], "N/A");
            }
          });

      if (axisChanged) {
        font.SetVariationValues(axisValue);
      }

      ImGui::EndDisabled();

      ImGui::SeparatorText("Font metrics");

      ImGui::LabelText("Ascend", "%.3f", font.Ascend());
      ImGui::LabelText("Descend", "%.3f", font.Descend());
      ImGui::LabelText("Line gap", "%.3f", font.LineGap());
      ImGui::LabelText("Line height", "%.3f", font.LineHeight());

      ImGui::SeparatorText("OpenType text shaping");
      ImGui::Checkbox("Enable##Shape Text", &isShaping);

      ImGui::BeginDisabled(!isShaping);
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

      constexpr magic_enum::containers::array<TextDirection, const char *>
          directionLabels{
              "Left to right",
              "Top to bottom",
#ifdef ENABLE_RTL
              "Right to left",
#endif
          };

      if (ImGui::BeginCombo("Direction", directionLabels[selectedDirection])) {
        magic_enum::enum_for_each<TextDirection>(
            [this, &directionLabels](const auto &dir) {
              if (ImGui::Selectable(directionLabels[dir],
                                    dir == selectedDirection)) {
                selectedDirection = dir;
              }
            });

        ImGui::EndCombo();
      }
      ImGui::EndDisabled();
    }

    if (ImGui::CollapsingHeader("Draw colors")) {
      auto f4Foreground = SDLColorToFloat4(foregroundColor);
      if (ImGui::ColorPicker3("Foreground##color", f4Foreground.data(),
                              ImGuiColorEditFlags_InputRGB)) {
        foregroundColor = Float4ToSDLColor(f4Foreground);
      };
      if (ImGui::Button("Reset##foreground color")) {
        foregroundColor = defaultForegroundColor;
      }

      auto f4Background = SDLColorToFloat4(backgroundColor);
      if (ImGui::ColorPicker3("Background##color", f4Background.data(),
                              ImGuiColorEditFlags_InputRGB)) {
        backgroundColor = Float4ToSDLColor(f4Background);
      };
      if (ImGui::Button("Reset##background color")) {
        backgroundColor = defaultBackgroundColor;
      }
    }
  }
  ImGui::End();

  if (context.showTextEditor) {
    if (ImGui::Begin("Input text", &context.showTextEditor)) {
      ImGui::InputTextMultiline("##InputText", buffer.data(), buffer.size());
    }
    ImGui::End();
  }

  if (context.debug) {
    if (ImGui::Begin("Debug", &context.debug)) {
      if (ImGui::CollapsingHeader("Features", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Checkbox("Baseline", &context.debugBaseline);
        ImGui::SameLine();
        ImGui::ColorButton(
            "Baseline", SDLColorToImVec4(debugBaselineColor),
            ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoPicker |
                ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoLabel);

        ImGui::Checkbox("Caret Positions", &context.debugCaret);
        ImGui::SameLine();
        ImGui::ColorButton(
            "Caret Positions", SDLColorToImVec4(debugCaretColor),
            ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoPicker |
                ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoLabel);

        ImGui::Checkbox("Glyph Bound", &context.debugGlyphBound);
        ImGui::SameLine();
        ImGui::ColorButton(
            "Glyph Bound", SDLColorToImVec4(debugGlyphBoundColor),
            ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoPicker |
                ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoLabel);

        ImGui::Checkbox("Ascend", &context.debugAscend);
        ImGui::SameLine();
        ImGui::ColorButton(
            "Ascend", SDLColorToImVec4(debugAscendColor),
            ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoPicker |
                ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoLabel);

        ImGui::Checkbox("Descend", &context.debugDescend);
        ImGui::SameLine();
        ImGui::ColorButton(
            "Descend", SDLColorToImVec4(debugDescendColor),
            ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoPicker |
                ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoLabel);
      }
    }
    ImGui::End();
  }

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
      if (!newFont.LoadFile(fontFilePaths[newSelected].string())) {
        ImGui::OpenPopup("InvalidFont");
      } else {
        font = newFont;
        axisLimits = font.GetAxisInfos();

        magic_enum::enum_for_each<VariationAxis>(
            [this](const VariationAxis &axis) {
              if (!axisLimits[axis].has_value())
                return;

              axisValue[axis] = axisLimits[axis]->defaultValue;
            });

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
  fontDirPath = path.string();
  fontFilePaths = ListFontFiles(fontDirPath);
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

    static auto compare = [](const char &c1, const char &c2) -> bool {
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

void MainScene::RenderText(SDL_Renderer *renderer, Context &ctx) {
  if (!font.IsValid())
    return;

  std::string str(buffer.data());
  auto language = languages[selectedLanguage].code;
  auto script = scripts[selectedScript].script;

  SDL_Color sdlColor = foregroundColor;

  if (!isShaping) {
    TextRenderNoShape(renderer, ctx, font, str, sdlColor);
    return;
  }

  switch (selectedDirection) {
  case TextDirection::LeftToRight:
    TextRenderLeftToRight(renderer, ctx, font, str, sdlColor, language, script);
    return;

  case TextDirection::TopToBottom:
    TextRenderTopToBottom(renderer, ctx, font, str, sdlColor, language, script);
    return;

#ifdef ENABLE_RTL
  case TextDirection::RightToLeft:
    TextRenderRightToLeft(renderer, ctx, font, str, sdlColor, language, script);
    return;
#endif
  }
}
