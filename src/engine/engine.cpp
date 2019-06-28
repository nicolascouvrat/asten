#include "engine.h"
#include "nes_engine.h"

Engine* Engine::newEngine() {
  return new NesEngine();
}
