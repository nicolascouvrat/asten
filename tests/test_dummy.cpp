#include <stdexcept>

void testSuccess() {
  // some stuff
}

void testFailure() {
  throw std::runtime_error("test failed");
}

int main() {
  testSuccess();
  testFailure();
}
