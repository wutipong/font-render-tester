#version 450 core

precision highp float;

in vec4 ex_Color;
out vec4 gl_FragColor;

void main(void) { gl_FragColor = ex_Color; }
