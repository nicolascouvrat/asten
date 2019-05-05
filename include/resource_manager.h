#ifndef GUARD_RESOURCE_MANAGER_H
#define GUARD_RESOURCE_MANAGER_H


#include <map>
#include <string>
#include <stdexcept>

#include <glad/glad.h>

#include "shader_program.h"


// ResourceManager that loads shader programs and stores them for future reference.
// All functions and resources are static.
class ResourceManager {
  public:
    class Error: public std::runtime_error {
      public:
        Error(const char* msg): std::runtime_error(msg) {}
    };
    static ShaderProgram createShaderProgram(std::string, const GLchar*, const GLchar*);
    static ShaderProgram getShaderProgram(std::string);
    // Always call this before destroying the GLFW context!
    static void deleteAll();
  private:
    ResourceManager() {}
    static std::map<std::string, ShaderProgram> shaderPrograms;
    static ShaderProgram loadShaderProgramFromFile(const GLchar*, const GLchar*);
    static std::string loadShaderCodeFromFile(const GLchar*);
};
#endif
