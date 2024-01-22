#include "main_scene.hpp"

#include "colors.hpp"
#include "text_renderer.hpp"
#include <algorithm>
#include <array>
#include <filesystem>
#include <imgui.h>
#include <imgui_internal.h>
#include <magic_enum_all.hpp>
#include <magic_enum_containers.hpp>

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

constexpr int toolbarWidth = 400;
constexpr int padding = 30;

} // namespace

bool MainScene::Init(Context &context) {
  if (!Font::Init())
    return false;

  dirChooser.SetTitle("Browse for font directory");
  OnDirectorySelected(context, context.fontPath);
  dirChooser.SetPwd(context.fontPath);

  std::copy(std::cbegin(exampleText), std::cend(exampleText), buffer.begin());

  return true;
}

void MainScene::Tick(SDL_Renderer *renderer, Context &ctx) {
  SDL_Rect viewport = ctx.windowBound;

  viewport.x += padding;
  viewport.y += padding;
  viewport.w -= (toolbarWidth + padding *2);
  viewport.h -= 2 * padding;

  SDL_RenderSetViewport(renderer, &viewport);

  SDL_SetRenderDrawColor(renderer, backgroundColor.r, backgroundColor.g,
                         backgroundColor.b, backgroundColor.a);
  SDL_RenderClear(renderer);

  font.SetFontSize(fontSize);

  SDL_Color sdlColor = {
      static_cast<uint8_t>(foregroundColor[0] * 255.0f),
      static_cast<uint8_t>(foregroundColor[1] * 255.0f),
      static_cast<uint8_t>(foregroundColor[2] * 255.0f),
      0xFF,
  };

  font.RenderText(renderer, ctx, std::string(buffer.data()), isShaping,
                  selectedDirection, sdlColor, languages[selectedLanguage].code,
                  scripts[selectedScript].script);

  SDL_RenderGetViewport(renderer, nullptr);
}

void MainScene::Cleanup(Context &context) { Font::CleanUp(); }

void MainScene::DoUI(Context &context) {
  int newSelected = selectedFontIndex;
  if (ImGui::BeginMainMenuBar()) {

    if (ImGui::BeginMenu("File##menu")) {
      if (ImGui::MenuItem("Change font directory##file-menu")) {
        dirChooser.Open();
      }

      if (ImGui::MenuItem("Re-scan font directory##file-menu")) {
        fontPaths = ListFontFiles(context.fontPath);
      }

      ImGui::Separator();

      if (ImGui::MenuItem("Exit", "Alt+F4")) {
        SDL_Event ev{};
        ev.quit.type = SDL_QUIT;
        ev.quit.timestamp = SDL_GetTicks();

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
      ImGui::LabelText("Font Directory", context.fontPath.c_str());
      ImGui::EndMenuBar();
    }
  }
  ImGui::End();

  if (ImGui::BeginViewportSideBar("toolbar", nullptr, ImGuiDir_Right,
                                  toolbarWidth,
                                  ImGuiWindowFlags_NoSavedSettings)) {

    if (ImGui::CollapsingHeader("Font", ImGuiTreeNodeFlags_DefaultOpen)) {
      auto currentFile = selectedFontIndex == -1
                             ? "<None>"
                             : fontPaths[selectedFontIndex].filename().string();

      if (ImGui::BeginCombo("Font file", currentFile.c_str())) {
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
              "Right to left",
              "Top to bottom",
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
    }
    ImGui::EndDisabled();

    ImGui::SeparatorText("Draw color");

    ImGui::ColorPicker3("Foreground##color", foregroundColor,
                        ImGuiColorEditFlags_InputRGB);
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
      if (!newFont.LoadFile(fontPaths[newSelected].string())) {
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
