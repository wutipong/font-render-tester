#version 450 core

precision highp float;

in vec4 ex_Color;
out vec4 FragColor;

void main(void) { FragColor = ex_Color; }
