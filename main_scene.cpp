#include "main_scene.hpp"

#include <imgui.h>

bool MainScene::Init(const Context &context) {

  dirChooser.SetTitle("Browse for font directory");

  return true;
}

void MainScene::Tick(const Context &context) {
  ImGui::Begin("Menu");
  {
    if (ImGui::CollapsingHeader("Font Directory",
                                ImGuiTreeNodeFlags_DefaultOpen)) {
      ImGui::Text(path.c_str());
      if (ImGui::Button("Browse")) {
        dirChooser.Open();
      }
    }

    if (ImGui::CollapsingHeader("Parameters", ImGuiTreeNodeFlags_DefaultOpen)) {
      if (ImGui::BeginCombo("Font", "Sarabun IT")) {
        ImGui::Selectable("Angsana UPC");
        ImGui::Selectable("Sarabun IT", true);
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
  { ImGui::InputText("text", buffer.data(), buffer.size()); }
  ImGui::End();

  dirChooser.Display();
  if (dirChooser.HasSelected()) {
    path = dirChooser.GetSelected().string();
    dirChooser.ClearSelected();
  }
}

void MainScene::Cleanup(const Context &context) {}
