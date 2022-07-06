#include "gl_util.hpp"
#include "io_util.hpp"
#include <spdlog/spdlog.h>

static void glMessageCallback(GLenum source, GLenum type, GLuint id,
                              GLenum severity, GLsizei length,
                              const GLchar *message, const void *userParam) {

  std::string sourceStr;
  switch (source) {
  case GL_DEBUG_SOURCE_API:
    sourceStr = "GL_DEBUG_SOURCE_API";
    break;
  case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
    sourceStr = "GL_DEBUG_SOURCE_WINDOW_SYSTEM";
    break;
  case GL_DEBUG_SOURCE_SHADER_COMPILER:
    sourceStr = "GL_DEBUG_SOURCE_SHADER_COMPILER";
    break;
  case GL_DEBUG_SOURCE_THIRD_PARTY:
    sourceStr = "GL_DEBUG_SOURCE_THIRD_PARTY";
    break;
  case GL_DEBUG_SOURCE_APPLICATION:
    sourceStr = "GL_DEBUG_SOURCE_APPLICATION";
    break;
  case GL_DEBUG_SOURCE_OTHER:
    sourceStr = "GL_DEBUG_SOURCE_OTHER";
    break;
  }

  std::string typeStr;
  switch (type) {
  case GL_DEBUG_TYPE_ERROR:
    typeStr = "GL_DEBUG_TYPE_ERROR";
    break;
  case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
    typeStr = "GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR";
    break;
  case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
    typeStr = "GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR";
    break;
  case GL_DEBUG_TYPE_PORTABILITY:
    typeStr = "GL_DEBUG_TYPE_PORTABILITY";
    break;
  case GL_DEBUG_TYPE_PERFORMANCE:
    typeStr = "GL_DEBUG_TYPE_PERFORMANCE";
    break;
  case GL_DEBUG_TYPE_MARKER:
    typeStr = "GL_DEBUG_TYPE_MARKER";
    break;
  case GL_DEBUG_TYPE_PUSH_GROUP:
    typeStr = "GL_DEBUG_TYPE_PUSH_GROUP";
    break;
  case GL_DEBUG_TYPE_POP_GROUP:
    typeStr = "GL_DEBUG_TYPE_POP_GROUP";
    break;
  case GL_DEBUG_TYPE_OTHER:
    typeStr = "GL_DEBUG_TYPE_OTHER";
    break;
  }

  switch (severity) {
  case GL_DEBUG_SEVERITY_HIGH:
    spdlog::error("[{}/{}]: {}", sourceStr, typeStr, message);
    break;

  case GL_DEBUG_SEVERITY_MEDIUM:
  case GL_DEBUG_SEVERITY_LOW:
    spdlog::warn("[{}/{}]: {}", sourceStr, typeStr, message);
    break;

  case GL_DEBUG_SEVERITY_NOTIFICATION:
    spdlog::warn("[{}/{}]: {}", sourceStr, typeStr, message);
    break;
  }
}

void SetupGLDebug() {
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(glMessageCallback, nullptr);
}

GLuint CompileShader(const std::filesystem::path &path, const std::string &name,
                     const GLenum &shaderType) {
  std::string shaderSrc;
  LoadFile(path, shaderSrc);
  auto shader = glCreateShader(shaderType);
  auto src = shaderSrc.c_str();
  glShaderSource(shader, 1, (const GLchar **)&src, 0);
  glCompileShader(shader);

  glObjectLabel(GL_SHADER, shader, name.length(), name.c_str());

  return shader;
}

void SetGLObjectLabel(GLenum identifier, GLuint name, const std::string label) {
  glObjectLabel(identifier, name, label.length(), label.c_str());
}
