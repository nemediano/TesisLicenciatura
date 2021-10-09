#include <iostream>
#include <stdexcept>
#include <cstdlib>

#include "SoftBodyApp.h"

int main (int argc, char* argv[]) {

  SoftBodyApp app;

  try {
      app.run();
  } catch (const std::exception& e) {
      std::cerr << e.what() << std::endl;
      return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
