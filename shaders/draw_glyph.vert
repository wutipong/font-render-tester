#version 450 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;

layout(location = 0) uniform vec2 screen;
layout(location = 1) uniform mat4 glyphTransform;
layout(location = 2) uniform mat4 transform;
layout(location = 3) uniform vec4 color;

out vec2 TexCoord;
out vec4 exColor;

void main() {
  vec4 pos = glyphTransform * vec4(aPos, 1.0f);
  pos = transform * pos;

  vec2 halfScreen = screen / 2;

  gl_Position = vec4((pos.x - halfScreen.x) / halfScreen.x,
                     (pos.y - halfScreen.y) / halfScreen.y, pos.z, pos.w);

  TexCoord = aTexCoord;
  exColor = color;
}
