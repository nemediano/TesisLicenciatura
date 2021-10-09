#ifndef SOFT_BODY_APP_H_
#define SOFT_BODY_APP_H_

// ANSI C++ includes
#include <array>
#include <fstream>
#include <vector>

// Third party libraries includes
// Dear imgui
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
// GLEW
#include <GL/glew.h>
// GLFW
#include <GLFW/glfw3.h>
// GLM
#define GLM_FORCE_PURE
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

// Includes from this template
#include "image/texture.h"
#include "image/screengrabber.h"
#include "mesh/model.h"

#include "ogl/oglprogram.h"
#include "ui/trackball.h"

#include "physics/SoftBody.h"
#include "physics/Ball.h"

#include "math/hrclock.h"

struct ProgramLocations {
  // Location for shader variables
  GLint uPVM;
  GLint uNormalMat;
  GLint uDiffuseMap;
  GLint uSpecularMap;
  GLint uAlpha;
  GLint aPosition;
  GLint aNormal;
  GLint aTextureCoord;
  // For wireframe
  GLint aForce;
  GLint uMaxForce;
  GLint uHighForceColor;
  GLint uLowForceColor;

  ProgramLocations() {
    reset();
  }

  void reset () {
    uPVM = -1;
    uNormalMat = -1;
    uDiffuseMap = -1;
    uSpecularMap = -1;
    uAlpha = -1;
    aPosition = -1;
    aNormal = -1;
    aTextureCoord = -1;
    aForce = -1;
    uMaxForce = -1;
    uHighForceColor = -1;
    uLowForceColor = -1;
  }
};


struct WindowState {
  GLFWmonitor* monitorPtr;
  int x_pos;
  int y_pos;
  int width;
  int height;
};

struct Timers {
  math::HRClock renderTime;    // 0
  math::HRClock integrateTime; // 1
  math::HRClock colisionsTime; // 2
  math::HRClock remeshTime;    // 3
  math::HRClock frameTime;     // 4
  std::array<double, 5> plotData;

  void reset () {
    renderTime.reset();
    integrateTime.reset();
    colisionsTime.reset();
    frameTime.reset();
  }

};

class SoftBodyApp {
  public:
    void run();
    // All the things that need to be access from the GLFW callbacks needs to be either public
    // or have accessor/mutator methods. Since I prefer simplicity I made them public

    //! To store the state of window (before going into full-screen) so it can return to it later
    WindowState mWinState;
    //! Main GLFW window and context handle
    GLFWwindow* mWinPtr;
    //! Switches between full-screen and windowed mode
    /*!
      Queries if the window is in windowed mode. If it is, then stores the current window state
      and switches to full-screen mode. If it is not (hence we are full-screen) then retrieves
      the previous window state (position and size) and returns to windowed mode
    */
    void change_window_mode();
    //! Reset softBody (for keyboard)
    void reset_soft_body() {
    	mBody.reset();
    }
    //! Object used to take screen-shoots
    image::ScreenGrabber mSg;
    //! Object used to control the trackball camera's rotation
    ui::Trackball mTrackBall;
    //! Keep track if we are in camera edit mode
    bool mMouseDrag = false;
    //! The current zoom level of the camera
    int mZoomLevel;
    //! Keep track of the current angle (in degree)
    float mCurrentAngle;
    //! Keep track if we render or not the user menu
    bool mShowMenu;
    //! Show hide DearImgui demo (if you need to quick look how to use a control)
    bool mShowDemoMenu;
    //! Stop/Play Animation
    bool mAnimation;
    //! Drop or not the ball
    bool mFall;
    //! Which render mode
    int mRenderMode;
    //! Integrator to use
    int mIntegrator;
  private:
    //! The soft body to be simulated
    physics::SoftBody mBody;
    //! The ball that fall into the soft body
    physics::Ball mBall;
    //! Ball's initial height
    float mBallInitialHeight = 1.5f;
    // See if we have a debug logger
    bool mHasDebug;
    // OGL shader porgram locations storage
    ProgramLocations mLocSolidProg;
    ProgramLocations mLocWireProg;
    // OpenGL program handler
    ogl::OGLProgram* mGLProgramSolidPtr = nullptr;
    // OpenGL program handlesr
    ogl::OGLProgram* mGLProgWireFramePtr = nullptr;
    // To keep track the elapsed time between frames
    double mLastTime = 0.0;
    // To keep track (for redraw) of the fabric vertices
    std::vector<mesh::Vertex> mFabricSolidVertices;
    std::vector<mesh::Vertex> mFabricWireVertices;
    // Vertex Array Object used to manage the Vertex Buffer Objects
    GLuint mVaoBoxSolid;
    GLuint mVaoFabricSolid;
    GLuint mVaoBoxWire;
	GLuint mVaoFabricWire;
    GLuint mVaoFloor;
    GLuint mVaoBall;
    // For the fabric vertices
    GLuint mFabricSolidVerticesVBO;
    GLuint mFabricWireVerticesVBO;
    // Texture's handlers
    image::Texture mGroundDiffuseMap;
    image::Texture mGroundSpecularMap;
    image::Texture mBoxDiffuseMap;
    image::Texture mBoxSpecularMap;
    image::Texture mBallDiffuseMap;
    image::Texture mBallSpecularMap;
    image::Texture mFabricDiffuseMap;
    image::Texture mFabricSpecularMap;
    size_t mNumIndicesWireBox;
    size_t mNumIndicesSolidBox;
    size_t mNumIndicesFloor;
    size_t mNumIndicesBall;
    size_t mNumIndicesWireFabric;
    size_t mNumIndicesSolidFabric;
    void init_glfw();
    void load_OpenGL();
    void init_program();
    void load_model_data_and_send_to_gpu();
    void render();
    void update();
    void free_resources();
    //! Setup the IMGUI library to work with current OpenGL and GLFW, also general look and feel
    void setup_menu();
    //! Create the actual user menu of the application and connect the application with it.
    /*!
      Needs to be called every frame.
      If you want to add/remove input controls, this is the place
    */
    void create_menu();
    // Related to callbacks
    //! Register all the GLFW callbacks with the only window
    /*!
      Uses the global window handler, to register all the functions with respective events
    */
    void register_glfw_callbacks();
    void create_box_solid_model();
    void create_box_wire_model();
    void create_ball_model();
    void create_floor_model();
    void create_fabric_solid_model();
    void create_fabric_wire_model();
    void render_box_solid(const glm::mat4& P, const glm::mat4& V);
    void render_box_wire(const glm::mat4& P, const glm::mat4& V);
    void render_ball(const glm::mat4& P, const glm::mat4& V);
    void render_floor(const glm::mat4& P, const glm::mat4& V);
    void render_fabric_solid(const glm::mat4& P, const glm::mat4& V);
    void render_fabric_wire(const glm::mat4& P, const glm::mat4& V);
    void update_fabric_mesh();
    void start_logging();
    void log_to_file();
    float mElapsed;
    float mMaxForceRange;
    Timers mStats;
    std::ofstream mFileLog;
    std::array<float, 5> mAccumulatorsFrame;
    std::array<float, 5> mAccumulatorsForce;
    size_t mFrameCounter;
};

#endif
