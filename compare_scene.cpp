#include "compare_scene.hpp"

#include <fstream>
#include <streambuf>
#include <vector>

#include <imgui.h>
#include <utf8.h>

#include "menu_scene.hpp"

bool CompareScene::Init(const Context &context) {
  std::basic_ifstream<unsigned char> file(fontFile, std::ios::binary);
  fontData =
      std::vector<unsigned char>{std::istreambuf_iterator<unsigned char>(file),
                                 std::istreambuf_iterator<unsigned char>()};

  file.close();

  if (!stbtt_InitFont(&fontInfo,
                      reinterpret_cast<unsigned char *>(fontData.data()), 0)) {
    return false;
  }

  hb_blob_t *blob =
      hb_blob_create(reinterpret_cast<char *>(fontData.data()), fontData.size(),
                     HB_MEMORY_MODE_READONLY, nullptr, nullptr);
  hb_face_t *hb_face = hb_face_create(blob, 0);
  hb_blob_destroy(blob); /* face will keep a reference to it */

  hb_font = hb_font_create(hb_face);

  std::fill(buffer.begin(), buffer.end(), 0);
  std::copy(std::begin(text), std::end(text), buffer.begin());

  return true;
}

void CompareScene::Tick(const Context &context) {
  ImGui::Begin("Menu");
  ImGui::InputText("text", buffer.data(), buffer.size());
  ImGui::SliderInt("font size", &fontSize, 0, 128);

  float c[4]{color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, 1.0};

  ImGui::ColorPicker4("color", c, ImGuiColorEditFlags_InputRGB);
  color.r = c[0] * 255;
  color.g = c[1] * 255;
  color.b = c[2] * 255;

  bool quit = ImGui::Button("Back");

  ImGui::End();

  if (quit) {
    ChangeScene<MenuScene>(context);

    return;
  }

  hb_font_set_scale(hb_font, fontSize * 64, fontSize * 64);
  float scale = stbtt_ScaleForPixelHeight(&fontInfo, fontSize);

  DrawText(buffer, color, 300, 300, fontInfo, scale, context.renderer);
  DrawTextHarfbuzz(buffer, color, 300, 500, fontInfo, scale, hb_font,
                   context.renderer);
}

void CompareScene::Cleanup(const Context &context) {
  hb_font_destroy(hb_font);
  fontData.clear();
}