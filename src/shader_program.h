#ifndef GUARD_SHADER_PROGRAM_H
#define GUARD_SHADER_PROGRAM_H


#include <stdexcept>
#include <glad/glad.h>


class ShaderProgram {
  public:
    class Error: public std::runtime_error {
      public:
        Error(const char* msg): std::runtime_error(msg) {}
    };
    
    class CompileError: public Error {
      public:
        CompileError(const char* msg): Error(msg) {}
    };
    class LinkingError: public Error {
      public:
        LinkingError(const char* msg): Error(msg) {}
    };

    ShaderProgram();
    // compile and link shader
    void compileAndLink(const GLchar*, const GLchar*);
    // activate shader
    void use();
    // uniform functions
    void setInt(const std::string&, int) const;
    void setBool(const std::string&, bool) const;
    void setFloat(const std::string&, float) const;
    // getter
    unsigned int getID();
  private:
    unsigned int ID;
    unsigned int compileShader(const GLchar*, int);
};

#endif
