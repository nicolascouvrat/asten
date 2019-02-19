#include "shader_program.h"

ShaderProgram::ShaderProgram(): ID(0) {}

unsigned int ShaderProgram::compileShader(const GLchar* shaderSource, int shaderType) {
  unsigned int shader = glCreateShader(shaderType);
  glShaderSource(shader, 1, &shaderSource, NULL);
  glCompileShader(shader);
  int success;
  char infoLog[512];
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(shader, 512, NULL, infoLog);
    throw CompileError(infoLog);
  }
  return shader;
}

void ShaderProgram::compileAndLink(const GLchar* vShaderSource, const GLchar* fShaderSource) {
  unsigned int vertexShader, fragmentShader;
  vertexShader = compileShader(vShaderSource, GL_VERTEX_SHADER);
  fragmentShader = compileShader(fShaderSource, GL_FRAGMENT_SHADER);

  ID = glCreateProgram();
  glAttachShader(ID, vertexShader);
  glAttachShader(ID, fragmentShader);
  glLinkProgram(ID);

  int success;
  char infoLog[512];
  glGetProgramiv(ID, GL_LINK_STATUS, &success);
  if (!success) {
      glGetProgramInfoLog(ID, 512, NULL, infoLog);
      throw LinkingError(infoLog);
  }
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
}

unsigned int ShaderProgram::getID() { return ID; }

void ShaderProgram::use() {
    glUseProgram(ID);
}

void ShaderProgram::setInt(const std::string& name, int value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void ShaderProgram::setBool(const std::string& name, bool value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void ShaderProgram::setFloat(const std::string& name, float value) const {
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}
