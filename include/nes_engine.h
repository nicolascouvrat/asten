#ifndef GUARD_NES_ENGINE_H
#define GUARD_NES_ENGINE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdexcept>
#include <chrono>
#include <array>
#include "resource_manager.h"
#include "shader_program.h"
#include "Logger.h"

struct Color {
  float r, g, b;
  constexpr Color(int c): 
    r((float)(c >> 16 & 0xff) / 256),
    g((float)(c >> 8 & 0xff) / 256),
    b((float)(c & 0xff) / 256)
  {}
};

const char* const WINDOW_NAME = "NES Emulator OwO";

class NesEngine {
  public:
    class Error: public std::runtime_error {
        public:
            Error(const char *msg): std::runtime_error(msg) {}
    };
    NesEngine();
    ~NesEngine();
    // Returns false if the window (i.e. the GLFW context) is down
    bool isRunning();
    // Calls rendering logic (flushes all changes to pixel color to the screen)
    void render();
    // Changes the pixel at coordinates X, Y to color C, C being and index in
    // the NES palette. Coordinates are left to right, top to bottom ((0,0)
    // being in the top left).
    void colorPixel(int, int, int);
    std::array<bool, 8> getButtons();
  private:
    static constexpr Color palette[64] = {
      Color(0x666666), Color(0x002A88), Color(0x1412A7), Color(0x3B00A4), Color(0x5C007E), Color(0x6E0040), Color(0x6C0600), Color(0x561D00),
      Color(0x333500), Color(0x0B4800), Color(0x005200), Color(0x004F08), Color(0x00404D), Color(0x000000), Color(0x000000), Color(0x000000),
      Color(0xADADAD), Color(0x155FD9), Color(0x4240FF), Color(0x7527FE), Color(0xA01ACC), Color(0xB71E7B), Color(0xB53120), Color(0x994E00),
      Color(0x6B6D00), Color(0x388700), Color(0x0C9300), Color(0x008F32), Color(0x007C8D), Color(0x000000), Color(0x000000), Color(0x000000),
      Color(0xFFFEFF), Color(0x64B0FF), Color(0x9290FF), Color(0xC676FF), Color(0xF36AFF), Color(0xFE6ECC), Color(0xFE8170), Color(0xEA9E22),
      Color(0xBCBE00), Color(0x88D800), Color(0x5CE430), Color(0x45E082), Color(0x48CDDE), Color(0x4F4F4F), Color(0x000000), Color(0x000000),
      Color(0xFFFEFF), Color(0xC0DFFF), Color(0xD3D2FF), Color(0xE8C8FF), Color(0xFBC2FF), Color(0xFEC4EA), Color(0xFECCC5), Color(0xF7D8A5),
      Color(0xE4E594), Color(0xCFEF96), Color(0xBDF4AB), Color(0xB3F3CC), Color(0xB5EBF2), Color(0xB8B8B8), Color(0x000000), Color(0x000000),
    };
    static const int NATIVE_NES_WIDTH = 256;
    static const int NATIVE_NES_HEIGHT = 240;
    static const int ZOOM_FACTOR = 3;
    static float adapt_width(int);
    static float adapt_height(int);
    Logger log;
    GLFWwindow *window;
    ShaderProgram shaderProgram;
    void processInput();
    void initWindow();
    void initShaderProgram();
    void initGrid();
    void initColor();
    void initVAO();
    void calculateFPS();
    float *offsets;
    float *colors;
    float quad[12];
    int frameCounter;
    std::chrono::time_point<std::chrono::high_resolution_clock> timeStamp;
    unsigned int quadVBO, colorVBO, offsetVBO, VAO;
};

#endif
