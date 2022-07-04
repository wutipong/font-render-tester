#pragma once

#include <GL/gl3w.h>
#include <GL/GL.h>
#include <string>
#include <filesystem>

void SetupGLDebug();
GLuint CompileShader(const std::filesystem::path& path, const std::string& name, const GLenum& shaderType);