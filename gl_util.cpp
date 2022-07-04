#include "gl_util.hpp"
#include "io_util.hpp"
#include <spdlog/spdlog.h>

static void glMessageCallback(GLenum source, GLenum type, GLuint id,
                              GLenum severity, GLsizei length,
                              const GLchar *message, const void *userParam) {
  switch (severity) {
  case GL_DEBUG_SEVERITY_HIGH:
    spdlog::error("OpenGL error : {}", message);
    break;
  case GL_DEBUG_SEVERITY_MEDIUM:
  case GL_DEBUG_SEVERITY_LOW:
    spdlog::warn("OpenGL warning: {}", message);
    break;

  case GL_DEBUG_SEVERITY_NOTIFICATION:
    spdlog::warn("OpenGL info: {}", message);
    break;
  }
}

void SetupGLDebug() {
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(glMessageCallback, nullptr);
}

GLuint CompileShader(const std::filesystem::path &path, const std::string& name, const GLenum &shaderType) {
  std::string shaderSrc;
  LoadFile(path, shaderSrc);
  auto shader = glCreateShader(shaderType);
  auto src = shaderSrc.c_str();
  glShaderSource(shader, 1, (const GLchar **)&src, 0);
  glCompileShader(shader);

  glObjectLabel(GL_SHADER, shader, name.length(), name.c_str());

  return shader;
}
