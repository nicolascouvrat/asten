#include "classic_interface.h"

#include <iostream>

#include "controller.h"


constexpr Color ClassicInterface::palette[64];

// XXX: this is a little weird, but lets us package our shaders in a more
// convenient way (no need to manually copy files and open them after as they
// will directly be added at preprocessing time).
// 
// It does require to add a R"()" (raw string) to the shader's source files.
const std::string ClassicInterface::fragmentShaderSource = 
  #include "classic_fragment_shader.fs"
;

const std::string ClassicInterface::vertexShaderSource =
  #include "classic_vertex_shader.vs"
;

ClassicInterface::ClassicInterface():
  log(Logger::getLogger("ClassicInterface")),
  quad{
    0.0f, 0.0f, // top left
    adaptWidth(ZOOM_FACTOR), 0.0f, // top right
    0.0f, -adaptHeight(ZOOM_FACTOR), // bottom left

    adaptWidth(ZOOM_FACTOR), 0.0f, // top right
    0.0f, -adaptHeight(ZOOM_FACTOR), // bottom left
    adaptWidth(ZOOM_FACTOR), -adaptHeight(ZOOM_FACTOR) // bottom right
  },
  timeStamp(std::chrono::high_resolution_clock::now())
{
  initWindow();
  initShaderProgram();
  initGrid();
  initColor();
  initVAO();
  log.setLevel(DEBUG);
  frameCounter = 0;
}

ClassicInterface::~ClassicInterface() {
  glfwTerminate();
  delete colors;
  delete offsets;
}

// Normalizes the [0, ZOOM_FACTOR * WIDTH] on the [0, 2] scale (which means that
// it will return correct size for the screen. However, the position requires a
// -1 to correct it).
float ClassicInterface::adaptWidth(int value) {
  float divisor = (float) 2 / (IOInterface::WIDTH * ZOOM_FACTOR);
  return (float) value * divisor;
}

float ClassicInterface::adaptHeight(int value) {
  float divisor = (float) 2 / (IOInterface::HEIGHT * ZOOM_FACTOR);
  return (float) value * divisor;
}

// Performs GLFW related initialization and spins up the window environment.
void ClassicInterface::initWindow() {
  // Initialize GLFW
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
#ifdef __APPLE__
  // this is necessary to compile on OSX
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
  // Initialize window
  int height = IOInterface::HEIGHT * ZOOM_FACTOR;
  int width = IOInterface::WIDTH * ZOOM_FACTOR;
  window = glfwCreateWindow(width, height, WINDOW_NAME, NULL, NULL);
  if (window == NULL)
    throw Error("Could not create game window.");
  glfwMakeContextCurrent(window);
  // Initialize GLAD
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    throw Error("Could not load GLAD.");
  glViewport(0, 0, width, height);
}

void ClassicInterface::initShaderProgram() {
  shaderProgram.compileAndLink(
    ClassicInterface::vertexShaderSource.c_str(),
    ClassicInterface::fragmentShaderSource.c_str()
  );
}

bool ClassicInterface::shouldClose() { return window == NULL || glfwWindowShouldClose(window); }

void ClassicInterface::calculateFPS() {
  auto now = std::chrono::high_resolution_clock::now();
  frameCounter++;
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - timeStamp);
  if (duration.count() > 1000) {
    log.debug() << "FPS: " << frameCounter << "\n";
    frameCounter = 0;
    timeStamp = now;
  }
}
// Handles rendering logic, namely:
//  1. Clear up the screen
//  2. Load new color data (as only this can be changed)
//  3. Draw instances
//  4. Handle events
void ClassicInterface::render() {
  glClearColor(1.0f, 0.0f, 0.1f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  int tileCount = IOInterface::WIDTH * IOInterface::HEIGHT;
  glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
  glBufferSubData(GL_ARRAY_BUFFER, 0, 3 * sizeof(float) * tileCount, colors);

  shaderProgram.use();
  glBindVertexArray(VAO);
  glDrawArraysInstanced(GL_TRIANGLES, 0, 6, IOInterface::WIDTH * IOInterface::HEIGHT);
  glBindVertexArray(0);
  calculateFPS();

  processInput();
  glfwSwapBuffers(window);
  glfwPollEvents();
}

void ClassicInterface::processInput() {
  // exit window on escape
  if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
}

bool ClassicInterface::shouldReset() {
  return glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS;
}

// Fills up the buttons
std::array<ButtonSet, 2> ClassicInterface::getButtons() {
  std::array<ButtonSet, 2> buttons = {0};
  buttons[0].A = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS;
  buttons[0].B = glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS;
  buttons[0].SELECT = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;
  buttons[0].START = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
  buttons[0].UP = glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS;
  buttons[0].DOWN = glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS;
  buttons[0].LEFT = glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS;
  buttons[0].RIGHT = glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS;
  return buttons;
}

// Creates the offsets that will be used to render the NES tiles (basically
// glorified pixels). Offsets are ordered LINE PER LINE, TOP TO BOTTOM and LEFT
// TO RIGHT.
void ClassicInterface::initGrid() {
  offsets = new float[IOInterface::WIDTH * IOInterface::HEIGHT * 2];
  int i = 0;
  float xOffset, yOffset;
  for (int y = 0; y < IOInterface::HEIGHT * ZOOM_FACTOR; y += ZOOM_FACTOR) {
    for (int x = 0; x < IOInterface::WIDTH * ZOOM_FACTOR; x += ZOOM_FACTOR) {
      // we fill it up line per line
      xOffset = adaptWidth(x) - 1;
      yOffset = -(adaptHeight(y) - 1);
      offsets[i++] = xOffset;
      offsets[i++] = yOffset;
    }
  }
}

// The color array has to be adjusted with the offset array!
void ClassicInterface::initColor() {
  colors = new float[IOInterface::WIDTH * IOInterface::HEIGHT * 3];
  bool isEven = false;
  float color = 0.0;
  float incrementC = (float) 1 / (IOInterface::WIDTH * IOInterface::HEIGHT);
  for (int i = 0; i < IOInterface::WIDTH * IOInterface::HEIGHT * 3; i += 3) {
    colors[i] = color;
    colors[i + 1] = color;
    colors[i + 2] = color;
    isEven = !isEven;
    color += incrementC;
  }
}

// X goes left to right, Y top to bottom
void ClassicInterface::colorPixel(int x, int y, int paletteIndex) {
  int index = (y * IOInterface::WIDTH + x) * 3;
  Color color = palette[paletteIndex];
  colors[index] = color.r; // red
  colors[index + 1] = color.g; // green
  colors[index + 2] = color.b; // blue
}

// Handles OpenGL related logic (creating, binding buffers and attribute
// pointers).
void ClassicInterface::initVAO() {
  int tileCount = IOInterface::WIDTH * IOInterface::HEIGHT;
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


