#include "nes_engine.h"


constexpr Color NesEngine::palette[64];


NesEngine::NesEngine(): quad{
    0.0f, 0.0f, // top left
    adapt_width(ZOOM_FACTOR), 0.0f, // top right
    0.0f, -adapt_height(ZOOM_FACTOR), // bottom left

    adapt_width(ZOOM_FACTOR), 0.0f, // top right
    0.0f, -adapt_height(ZOOM_FACTOR), // bottom left
    adapt_width(ZOOM_FACTOR), -adapt_height(ZOOM_FACTOR) // bottom right
}
{
  initWindow();
  initShaderProgram();
  initGrid();
  initColor();
  initVAO();
}

NesEngine::~NesEngine() {
  glfwTerminate();
  delete colors;
  delete offsets;
}

// Normalizes the [0, ZOOM_FACTOR * WIDTH] on the [0, 2] scale (which means that
// it will return correct size for the screen. However, the position requires a
// -1 to correct it).
float NesEngine::adapt_width(int value) {
  float divisor = (float) 2 / (NATIVE_NES_WIDTH * ZOOM_FACTOR);
  return (float) value * divisor;
}

float NesEngine::adapt_height(int value) {
  float divisor = (float) 2 / (NATIVE_NES_HEIGHT * ZOOM_FACTOR);
  return (float) value * divisor;
}

// Performs GLFW related initialization and spins up the window environment.
void NesEngine::initWindow() {
  // Initialize GLFW
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
  // Initialize window
  int height = NATIVE_NES_HEIGHT * ZOOM_FACTOR;
  int width = NATIVE_NES_WIDTH * ZOOM_FACTOR;
  window = glfwCreateWindow(width, height, WINDOW_NAME, NULL, NULL);
  if (window == NULL)
    throw Error("Could not create game window.");
  glfwMakeContextCurrent(window);
  // Initialize GLAD
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    throw Error("Could not load GLAD.");
  glViewport(0, 0, width, height);
}

void NesEngine::initShaderProgram() {
  shaderProgram = ResourceManager::createShaderProgram(
      "nes_shader_program", "src/nes_vertex_shader.vs", "src/nes_fragment_shader.fs");
}

bool NesEngine::isRunning() { return window != NULL && !glfwWindowShouldClose(window); }

// Handles rendering logic, namely:
//  1. Clear up the screen
//  2. Load new color data (as only this can be changed)
//  3. Draw instances
//  4. Handle events
void NesEngine::render() {
  glClearColor(1.0f, 0.0f, 0.1f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  int tileCount = NATIVE_NES_WIDTH * NATIVE_NES_HEIGHT;
  glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
  glBufferSubData(GL_ARRAY_BUFFER, 0, 3 * sizeof(float) * tileCount, colors);

  shaderProgram.use();
  glBindVertexArray(VAO);
  glDrawArraysInstanced(GL_TRIANGLES, 0, 6, NATIVE_NES_WIDTH * NATIVE_NES_HEIGHT);
  glBindVertexArray(0);

  processInput();
  glfwSwapBuffers(window);
  glfwPollEvents();
}

void NesEngine::processInput() {
  // exit window on escape
  if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
}

// Creates the offsets that will be used to render the NES tiles (basically
// glorified pixels). Offsets are ordered LINE PER LINE, TOP TO BOTTOM and LEFT
// TO RIGHT.
void NesEngine::initGrid() {
  offsets = new float[NATIVE_NES_WIDTH * NATIVE_NES_HEIGHT * 2];
  int i = 0;
  float xOffset, yOffset;
  for (int y = 0; y < NATIVE_NES_HEIGHT * ZOOM_FACTOR; y += ZOOM_FACTOR) {
    for (int x = 0; x < NATIVE_NES_WIDTH * ZOOM_FACTOR; x += ZOOM_FACTOR) {
      // we fill it up line per line
      xOffset = adapt_width(x) - 1;
      yOffset = -(adapt_height(y) - 1);
      offsets[i++] = xOffset;
      offsets[i++] = yOffset;
    }
  }
}

// The color array has to be adjusted with the offset array!
void NesEngine::initColor() {
  colors = new float[NATIVE_NES_WIDTH * NATIVE_NES_HEIGHT * 3];
  bool is_even = false;
  float color = 0.0;
  float increment_c = (float) 1 / (NATIVE_NES_WIDTH * NATIVE_NES_HEIGHT);
  for (int i = 0; i < NATIVE_NES_WIDTH * NATIVE_NES_HEIGHT * 3; i += 3) {
    // color = (is_even) ? 1.0 : 0.0;
    colors[i] = color;
    colors[i + 1] = color;
    colors[i + 2] = color;
    is_even = !is_even;
    color += increment_c;
  }
}

// X goes left to right, Y top to bottom
void NesEngine::colorPixel(int x, int y, int palette_index) {
  int index = (y * NATIVE_NES_WIDTH + x) * 3;
  Color color = palette[palette_index];
  colors[index] = color.r; // red
  colors[index + 1] = color.g; // green
  colors[index + 2] = color.b; // blue
}

// Handles OpenGL related logic (creating, binding buffers and attribute
// pointers).
void NesEngine::initVAO() {
  int tileCount = NATIVE_NES_WIDTH * NATIVE_NES_HEIGHT;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &quadVBO);
  glGenBuffers(1, &colorVBO);
  glGenBuffers(1, &offsetVBO);

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 12, quad, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);


  glBindBuffer(GL_ARRAY_BUFFER, offsetVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * tileCount * 2, offsets, GL_STATIC_DRAW);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

  glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * tileCount * 3, colors, GL_DYNAMIC_DRAW);
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);


  glVertexAttribDivisor(1, 1);
  glVertexAttribDivisor(2, 1);
}


