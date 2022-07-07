#pragma once

#include <GL/gl3w.h>
#include <string>
#include <filesystem>

void SetupGLDebug();
GLuint CompileShader(const std::filesystem::path& path, const std::string& name, const GLenum& shaderType);
void SetGLObjectLabel(GLenum identifier, GLuint name, const std::string label);
