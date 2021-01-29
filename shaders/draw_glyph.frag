#version 450 core
out vec4 FragColor;

in vec2 TexCoord;
in vec4 exColor;

layout(binding = 0) uniform sampler2D alphaMap;

void main() {
  float alpha = texture(alphaMap, TexCoord).r;

  FragColor = exColor;
  FragColor.a = exColor.a * alpha;
}
