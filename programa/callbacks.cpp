#include <iostream>
#include <sstream>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"

#include "SoftBodyApp.h"

#include "callbacks.h"

void SoftBodyApp::register_glfw_callbacks() {
  // Associate the class instance so, it can be acceses from
  // inside the static callback functions
  glfwSetWindowUserPointer(mWinPtr, static_cast<void*>(this));
  // Register the callback functions
  glfwSetWindowSizeCallback(mWinPtr, resize_callback);
  glfwSetFramebufferSizeCallback(mWinPtr, framebuffer_size_callback);
  glfwSetKeyCallback(mWinPtr, key_callback);
  glfwSetMouseButtonCallback(mWinPtr, mouse_button_callback);
  glfwSetCursorPosCallback(mWinPtr, cursor_position_callback);
  glfwSetScrollCallback(mWinPtr, scroll_callback);
}

void key_callback(GLFWwindow* windowPtr, int key, int scancode, int action, int mods) {
  ImGuiIO& io = ImGui::GetIO();
  //Imgui wants this event, since it happen inside the GUI
  if (io.WantCaptureKeyboard) {
    return;
  }
  // Get reference to the main class instance
  SoftBodyApp* app = static_cast<SoftBodyApp*>(glfwGetWindowUserPointer(windowPtr));
  //The event happen outside the GUI, your application should try to handle it
  if (action == GLFW_PRESS) {
    switch (key) {
      case GLFW_KEY_ESCAPE:
        glfwSetWindowShouldClose(windowPtr, 1);
      break;
      case GLFW_KEY_F11:
        app->change_window_mode();
      break;
      case GLFW_KEY_S:
        app->mSg.grab();
      break;
      case GLFW_KEY_M:
        app->mShowMenu = !(app->mShowMenu);
      break;
      /*case GLFW_KEY_D:
        app->mShowDemoMenu = !(app->mShowDemoMenu);
      break;*/
      case GLFW_KEY_P:
    	  app->mAnimation = !(app->mAnimation);
	  break;
      case GLFW_KEY_R:
		  app->reset_soft_body();
	  break;
    }
  }
}

void mouse_button_callback(GLFWwindow* windowPtr, int button, int action, int mods) {
  ImGuiIO& io = ImGui::GetIO();
  //Imgui wants this event, since it happen inside the GUI
  if (io.WantCaptureMouse) {
    return;
  }
  // Get reference to the main class instance
  SoftBodyApp* app = static_cast<SoftBodyApp*>(glfwGetWindowUserPointer(windowPtr));
  //The event happen outside the GUI, your application should try to handle it
  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
    // Start the mouse dragging event (edit camera mode)
    app->mMouseDrag = true;
    double mouse_x;
    double mouse_y;
    glfwGetCursorPos(windowPtr, &mouse_x, &mouse_y);
    app->mTrackBall.startDrag(glm::ivec2(int(mouse_x), int(mouse_y)));
  } else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
    // Stops the mouse dragging (exit edit camera mode)
    app->mTrackBall.endDrag();
    app->mMouseDrag = false;
  }
}

void cursor_position_callback(GLFWwindow* windowPtr, double mouse_x, double mouse_y) {
  ImGuiIO& io = ImGui::GetIO();
  //Imgui wants this event, since it happen inside the GUI
  if (io.WantCaptureMouse) {
    return;
  }
  // Get reference to the main class instance
  SoftBodyApp* app = static_cast<SoftBodyApp*>(glfwGetWindowUserPointer(windowPtr));
  if (app->mMouseDrag) {
    // If we are editing the camera, let the trackball update too
    app->mTrackBall.drag(glm::ivec2(int(mouse_x), int(mouse_y)));
  }
}

void scroll_callback(GLFWwindow* windowPtr, double x_offset, double y_offset) {
  ImGuiIO& io = ImGui::GetIO();
  if (io.WantCaptureMouse) {
    ImGui_ImplGlfw_ScrollCallback(windowPtr, x_offset, y_offset);
    return;
  }
  // Get reference to the main class instance
  SoftBodyApp* app = static_cast<SoftBodyApp*>(glfwGetWindowUserPointer(windowPtr));
  // If the user actiave the mouse wheel, change the zoom level
  app->mZoomLevel += int(y_offset);
  // zoom level is an integer between [-5, 3]
  app->mZoomLevel = glm::clamp(app->mZoomLevel, -5, 3);
}

void resize_callback(GLFWwindow* windowPtr, int new_window_width, int new_window_height) {
  // Update OpenGl viewport
  glViewport(0, 0, new_window_width, new_window_height);
  // Get reference to the main class instance
  SoftBodyApp* app = static_cast<SoftBodyApp*>(glfwGetWindowUserPointer(windowPtr));
  // Update the trackball size
  app->mTrackBall.setWindowSize(new_window_width, new_window_height);
}

void framebuffer_size_callback(GLFWwindow* windowPtr, int width, int height) {
  // Get reference to the main class instance
  SoftBodyApp* app = static_cast<SoftBodyApp*>(glfwGetWindowUserPointer(windowPtr));
  // Update the screengrabber resolution, I do this here an not in the resize
  // because the resolution could change without the window size changed
  // For example, if the window moves into a higer resolution display
  // The screen grabber needs to match the framebuffer (not the window size)
  app->mSg.resize(width, height);
}

void glfw_error_callback(int error, const char* description) {
  /* If this function is executed an internal GLFW error was generated and the program will likely
  crash. The most common failure, is that the application tries to create an OpenGL context superior
  to the one actually supported by your SW (driver) and HW (GPU) */
  std::stringstream ss;
  ss << "GLFW Error (" << error << "): " << description << std::endl;
  // Print the error to console as it is
  std::cerr << ss.str();
  //throw std::runtime_error(ss.str());
}

void SoftBodyApp::change_window_mode() {
  //Windowed windows return null as their monitor
  GLFWmonitor* monitor = glfwGetWindowMonitor(mWinPtr);

  if (monitor) { // Go to windowed mode
    glfwSetWindowMonitor(mWinPtr, nullptr, mWinState.x_pos, mWinState.y_pos,
        mWinState.width, mWinState.height, 0);
  } else { // go to full screen
    // Store you current state
    glfwGetWindowPos(mWinPtr, &mWinState.x_pos, &mWinState.y_pos);
    glfwGetWindowSize(mWinPtr, &mWinState.width, &mWinState.height);
    mWinState.monitorPtr = find_best_monitor(mWinPtr);
    // Now go, to full-screnn mode
    const GLFWvidmode* mode = glfwGetVideoMode(mWinState.monitorPtr);
    glfwSetWindowMonitor(mWinPtr, mWinState.monitorPtr, 0, 0, mode->width,
        mode->height, mode->refreshRate);
  }
}

/**
 * I took this algorithm (and idea) and adapted the code from here:
 * http://stackoverflow.com/questions/21421074/
 *     how-to-create-a-full-screen-window-on-the-current-monitor-with-glfw
 * Returns the monitor which contains the greater area of the window
 */
GLFWmonitor* find_best_monitor(GLFWwindow *windowPtr) {
  using glm::min;
  using glm::max;
  // Get window's info: position and size
  glm::ivec2 winPos;
  glm::ivec2 winSize;
  glfwGetWindowPos(windowPtr, &winPos.x, &winPos.y);
  glfwGetWindowSize(windowPtr, &winSize.x, &winSize.y);
  // Query the number of monitors and get handle of them
  int num_monitors = 0;
  GLFWmonitor **monitors = glfwGetMonitors(&num_monitors);
  // The initial best value is the minimun possible, i.e: no overlaping
  int best_overlap = 0;
  GLFWmonitor *best_monitor = nullptr;

  // Loop checking all the available monitors to see
  // if one of them has better overlaping area.
  for (int i = 0; i < num_monitors; i++) {
    // Get current monitor's info
    glm::ivec2 monPos;
    glm::ivec2 monSize;
    glfwGetMonitorPos(monitors[i], &monPos.x, &monPos.y);
    const GLFWvidmode *mode = glfwGetVideoMode(monitors[i]);
    monSize.x = mode->width;
    monSize.y = mode->height;
    // Calculate the area of overlap between this monitor and the window
    // The overlap is always a rectangle, so area equals height times width
    int overlap =
      max(0, min(winPos.x + winSize.x, monPos.x + monSize.x) - max(winPos.x, monPos.x)) * // width
      max(0, min(winPos.y + winSize.y, monPos.y + monSize.y) - max(winPos.y, monPos.y));  // height
    // If this area is better than our current best, then this is the new best
    if (best_overlap < overlap) {
      best_overlap = overlap;
      best_monitor = monitors[i];
    }
  }

  return best_monitor;
}
