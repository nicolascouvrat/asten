#include "resource_manager.h"
#include <sstream>
#include <fstream>
#include <iostream>

std::map<std::string, ShaderProgram> ResourceManager::shaderPrograms;

ShaderProgram ResourceManager::createShaderProgram(
    std::string name, const GLchar* vShaderFile, const GLchar* fShaderFile
) {
  auto it = shaderPrograms.emplace(std::make_pair( 
      name, loadShaderProgramFromFile(vShaderFile, fShaderFile)));
  return (it.first)->second;
}

ShaderProgram ResourceManager::getShaderProgram(std::string name) {
  return shaderPrograms[name];
}

ShaderProgram ResourceManager::loadShaderProgramFromFile(
    const GLchar *vShaderFile, const GLchar* fShaderFile) {
  std::string vShaderCode_ = loadShaderCodeFromFile(vShaderFile);
  std::string fShaderCode_ = loadShaderCodeFromFile(fShaderFile);
  const GLchar* vShaderCode = vShaderCode_.c_str();
  const GLchar* fShaderCode = fShaderCode_.c_str();
  ShaderProgram shaderProgram;
  shaderProgram.compileAndLink(vShaderCode, fShaderCode);
  return shaderProgram;
}

std::string ResourceManager::loadShaderCodeFromFile(const GLchar *path) {
  std::string shaderCode;
  std::ifstream shaderFile;

  shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  try {
    shaderFile.open(path);
    std::stringstream shaderStream;
    shaderStream << shaderFile.rdbuf();
    shaderFile.close();
    shaderCode = shaderStream.str();
  }
  catch (std::ifstream::failure e) {
    throw Error("Could not load shader file.");
  }
  
  return shaderCode;
}

void ResourceManager::deleteAll() {
  for (auto it : shaderPrograms)
    glDeleteProgram(it.second.getID());
}
