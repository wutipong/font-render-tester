#version 450 core

layout(location = 0) in vec2 in_Position;

layout(location = 0) uniform vec2 screen;
layout(location = 1) uniform vec4 in_Color;
layout(location = 2) uniform mat4 transform;

out vec4 ex_Color;

void main(void) {
  vec4 pos = transform * vec4(in_Position.x, in_Position.y, 0.0, 1.0);

  vec2 halfScreen = screen / 2;
  gl_Position = vec4((pos.x - halfScreen.x) / halfScreen.x,
                     (pos.y - halfScreen.y) / halfScreen.y, pos.z, pos.w);

  ex_Color = in_Color;
}
