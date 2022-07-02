#pragma once

#include <GL/GL.h>
#include <GL/GLU.h>
#include <string>

#define LogGlOperation(level, op) \
    (op);                         \
    auto str = GetGlErrorString();\
    spdlog::level("{}: {}", #op, str)

static std::string GetGlErrorString() {
  auto errCode = glGetError();

  switch (errCode) {
  case GL_NO_ERROR:
    return "GL_NO_ERROR";
  case GL_INVALID_ENUM:
    return "GL_INVALID_ENUM";
  case GL_INVALID_VALUE:
    return "GL_INVALID_VALUE";
  case GL_INVALID_OPERATION:
    return "GL_INVALID_OPERATION";
  case GL_STACK_OVERFLOW:
    return "GL_STACK_OVERFLOW";
  case GL_STACK_UNDERFLOW:
    return "GL_STACK_UNDERFLOW";
  case GL_OUT_OF_MEMORY:
    return "GL_OUT_OF_MEMORY";
  case GL_INVALID_FRAMEBUFFER_OPERATION:
    return "GL_INVALID_FRAMEBUFFER_OPERATION";
  case GL_CONTEXT_LOST:
    return "GL_CONTEXT_LOST";
  default:
    return "";
  }
}
