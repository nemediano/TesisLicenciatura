//Standard libraries includes
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
// Third party libraries includes
// GLM
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//Includes from this template
#include "ogl/oglhelpers.h"
#include "mesh/proceduralmeshes.h"

// Includes from this project
#include "callbacks.h"
#include "SoftBodyApp.h"

void SoftBodyApp::init_glfw() {
  using std::cerr;
  using std::endl;
  // Set error log for GLFW
  glfwSetErrorCallback(glfw_error_callback);
  // Try to init library
  if (!glfwInit()) {
    // Initialization failed
    cerr << "GLFW initialization failed!" << endl;
    throw std::runtime_error("GLFW initialization failed!");
  }
  // Library was initializated, now try window and context
  // This depends on the HW (GPU) and SW (Driver), use the best available
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  // This lines require OpenGL 4.3 or above, comment them if you don't have it
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

  mWinPtr = glfwCreateWindow(900, 600, "Soft Body simulation", nullptr, nullptr);
  if (!mWinPtr) {
    // Window or context creation failed
    glfwTerminate();
    throw std::runtime_error("OpenGL context not available");
  }
  // Save window state
  mWinState.monitorPtr = glfwGetPrimaryMonitor();
  // Context setting needs to happen before OpenGL's extension loader
  glfwMakeContextCurrent(mWinPtr);
  glfwSwapInterval(1);
}

void SoftBodyApp::load_OpenGL() {
  // Init OpenGL's context
  GLenum err = glewInit();
  if (GLEW_OK != err) {
    std::stringstream ss;
    ss << "Glew initialization failed: " << glewGetErrorString(err) << std::endl;
    throw std::runtime_error(ss.str());
  }
  // If our context allow us, ask for a debug callback
  mHasDebug = ogl::getErrorLog();
}

void SoftBodyApp::init_program() {
  // create OGL program for Solid Render
  mGLProgramSolidPtr = new ogl::OGLProgram("shaders/vertex.glsl", "shaders/fragment.glsl");
  // Now, that we have the program, query location of shader variables */
  mLocSolidProg.uPVM = mGLProgramSolidPtr->uniformLoc("PVM");
  mLocSolidProg.uNormalMat = mGLProgramSolidPtr->uniformLoc("NormalMat");
  mLocSolidProg.uAlpha = mGLProgramSolidPtr->uniformLoc("uAlpha");
  mLocSolidProg.uDiffuseMap = mGLProgramSolidPtr->uniformLoc("uDiffuseMap");
  mLocSolidProg.uSpecularMap = mGLProgramSolidPtr->uniformLoc("uSpecularMap");
  mLocSolidProg.aPosition = mGLProgramSolidPtr->attribLoc("posAttr");
  mLocSolidProg.aNormal = mGLProgramSolidPtr->attribLoc("normalAttr");
  mLocSolidProg.aTextureCoord = mGLProgramSolidPtr->attribLoc("textCoordAttr");

  mGLProgWireFramePtr = new ogl::OGLProgram("shaders/vertexWire.glsl", "shaders/fragmentWire.glsl");
  mLocWireProg.uPVM = mGLProgWireFramePtr->uniformLoc("PVM");
  mLocWireProg.uMaxForce = mGLProgWireFramePtr->uniformLoc("uMaxForce");
  mLocWireProg.uLowForceColor = mGLProgWireFramePtr->uniformLoc("uLowForceColor");
  mLocWireProg.uHighForceColor = mGLProgWireFramePtr->uniformLoc("uHighForceColor");
  mLocWireProg.aPosition = mGLProgWireFramePtr->attribLoc("posAttr");
  mLocWireProg.aForce = mGLProgWireFramePtr->attribLoc("forceAttr");

  // Then, create primitives and send data to GPU
  load_model_data_and_send_to_gpu();
  // Initialize some basic rendering state
  glEnable(GL_DEPTH_TEST);
  glClearColor(0.15f, 0.15f, 0.15f, 1.0f); // Dark gray background color
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glEnable(GL_PROGRAM_POINT_SIZE);
  // Initialize trackball camera
  int width;
  int height;
  glfwGetWindowSize(mWinPtr, &width, &height);
  mTrackBall.setWindowSize(width, height);
  // Also, let the screen grabber know the current buffer size
  mSg.resize(width, height);
  // Initial values for program logic
  mShowMenu = true;
  mShowDemoMenu = false;
  mZoomLevel = -2;
  mRenderMode = 1;
  //Initial value for the physical parameters
  mAnimation = true;
  mIntegrator = 2;
  mFall = false;
  mBall.setRadious(0.25f);
  mBall.setMass(0.6f);
  mBall.move(glm::vec3(0.0f, mBallInitialHeight, 0.0f));
  mMaxForceRange = 0.75f; // This value was determined experimentally
}

void SoftBodyApp::load_model_data_and_send_to_gpu() {
  // Create the soft body models
  create_box_solid_model();
  create_box_wire_model();
  create_fabric_solid_model();
  create_fabric_wire_model();
  // Now the ground
  create_floor_model();
  // Now the sphere
  create_ball_model();
}

void SoftBodyApp::create_box_solid_model() {
  using namespace mesh;
  // Create model
  Mesh boxMesh = mBody.getBoxSolidMesh();
  mNumIndicesSolidBox = boxMesh.indicesCount();
  // Query data
  std::vector<unsigned int> indices = boxMesh.getIndices();
  std::vector<Vertex> vertices = boxMesh.getVertices();
  // Load textures and send them to GPU
  mBoxSpecularMap = image::Texture("models/container2.png");
  mBoxSpecularMap.send_to_gpu();
  mBoxDiffuseMap = image::Texture("models/container2_specular.png");
  mBoxDiffuseMap.send_to_gpu();
  // Create the vertex buffer objects and VAO
  GLuint vertexBuffer;
  GLuint indexBuffer;
  glGenVertexArrays(1, &mVaoBoxSolid);
  glGenBuffers(1, &vertexBuffer);
  glGenBuffers(1, &indexBuffer);
  // Bind the vao this need to be done before anything
  glBindVertexArray(mVaoBoxSolid);
  // Send data to GPU: first send the vertices
  glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
  glEnableVertexAttribArray(mLocSolidProg.aPosition);
  glVertexAttribPointer(mLocSolidProg.aPosition, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        OFFSET_OF(Vertex, position));
  glEnableVertexAttribArray(mLocSolidProg.aNormal);
  glVertexAttribPointer(mLocSolidProg.aNormal, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          OFFSET_OF(Vertex, normal));
  glEnableVertexAttribArray(mLocSolidProg.aTextureCoord);
  glVertexAttribPointer(mLocSolidProg.aTextureCoord, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          OFFSET_OF(Vertex, textCoords));
  // Now, the indices
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
               indices.data(), GL_STATIC_DRAW);
  // Unbind the vao we will use it for render
  glBindVertexArray(0);
  // Now that we have the data in the GPU and the reference in vao we do not need to keep it
  glDeleteBuffers(1, &vertexBuffer);
  glDeleteBuffers(1, &indexBuffer);
  vertices.clear();
  indices.clear();
}

void SoftBodyApp::create_box_wire_model() {
  using namespace mesh;
  // Create model
  Mesh boxMesh = mBody.getBoxWireMesh();
  mNumIndicesWireBox = boxMesh.indicesCount();
  // Query data
  std::vector<unsigned int> indices = boxMesh.getIndices();
  std::vector<Vertex> vertices = boxMesh.getVertices();
  // Create the vertex buffer objects and VAO
  GLuint vertexBuffer;
  GLuint indexBuffer;
  glGenVertexArrays(1, &mVaoBoxWire);
  glGenBuffers(1, &vertexBuffer);
  glGenBuffers(1, &indexBuffer);
  // Bind the vao this need to be done before anything
  glBindVertexArray(mVaoBoxWire);
  // Send data to GPU: first send the vertices
  glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
  glEnableVertexAttribArray(mLocWireProg.aPosition);
  glVertexAttribPointer(mLocWireProg.aPosition, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
						OFFSET_OF(Vertex, position));
  glEnableVertexAttribArray(mLocWireProg.aForce);
  glVertexAttribPointer(mLocWireProg.aForce, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
						  OFFSET_OF(Vertex, normal));
  // Now, the indices
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
			   indices.data(), GL_STATIC_DRAW);
  // Unbind the vao we will use it for render
  glBindVertexArray(0);
  // Now that we have the data in the GPU and the reference in vao we do not need to keep it
  glDeleteBuffers(1, &vertexBuffer);
  glDeleteBuffers(1, &indexBuffer);
  vertices.clear();
  indices.clear();
}

void SoftBodyApp::create_ball_model() {
  using namespace mesh;
  // Create model
  Mesh sphereMesh = sphere();
  mNumIndicesBall = sphereMesh.indicesCount();
  // Query data
  std::vector<unsigned int> indices = sphereMesh.getIndices();
  std::vector<Vertex> vertices = sphereMesh.getVertices();
  // Load textures and send them to GPU
  mBallDiffuseMap = image::Texture("models/ball_texture.png");
  mBallDiffuseMap.send_to_gpu();
  mBallSpecularMap = image::Texture("models/ball_specular.png");
  mBallSpecularMap.send_to_gpu();
  // Create the vertex buffer objects and VAO
  GLuint vertexBuffer;
  GLuint indexBuffer;
  glGenVertexArrays(1, &mVaoBall);
  glGenBuffers(1, &vertexBuffer);
  glGenBuffers(1, &indexBuffer);
  // Bind the vao this need to be done before anything
  glBindVertexArray(mVaoBall);
  // Send data to GPU: first send the vertices
  glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
  glEnableVertexAttribArray(mLocSolidProg.aPosition);
  glVertexAttribPointer(mLocSolidProg.aPosition, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        OFFSET_OF(Vertex, position));
  glEnableVertexAttribArray(mLocSolidProg.aNormal);
  glVertexAttribPointer(mLocSolidProg.aNormal, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          OFFSET_OF(Vertex, normal));
  glEnableVertexAttribArray(mLocSolidProg.aTextureCoord);
  glVertexAttribPointer(mLocSolidProg.aTextureCoord, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          OFFSET_OF(Vertex, textCoords));
  // Now, the indices
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
               indices.data(), GL_STATIC_DRAW);
  // Unbind the vao we will use it for render
  glBindVertexArray(0);
  // Now that we have the data in the GPU and the reference in vao we do not need to keep it
  glDeleteBuffers(1, &vertexBuffer);
  glDeleteBuffers(1, &indexBuffer);
  vertices.clear();
  indices.clear();
}

void SoftBodyApp::create_floor_model() {
  using namespace mesh;
  // Create model
  Mesh planeMesh = plane(10, true);
  mNumIndicesFloor = planeMesh.indicesCount();
  // Query data
  std::vector<unsigned int> indices = planeMesh.getIndices();
  std::vector<Vertex> vertices = planeMesh.getVertices();
  // Load textures and send them to GPU
  mGroundDiffuseMap = image::Texture("models/grass.png");
  mGroundDiffuseMap.send_to_gpu();
  mGroundSpecularMap = image::Texture("models/grass_specular.png");
  mGroundSpecularMap.send_to_gpu();
  // Create the vertex buffer objects and VAO
  GLuint vertexBuffer;
  GLuint indexBuffer;
  glGenVertexArrays(1, &mVaoFloor);
  glGenBuffers(1, &vertexBuffer);
  glGenBuffers(1, &indexBuffer);
  // Bind the vao this need to be done before anything
  glBindVertexArray(mVaoFloor);
  // Send data to GPU: first send the vertices
  glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
  glEnableVertexAttribArray(mLocSolidProg.aPosition);
  glVertexAttribPointer(mLocSolidProg.aPosition, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        OFFSET_OF(Vertex, position));
  glEnableVertexAttribArray(mLocSolidProg.aNormal);
  glVertexAttribPointer(mLocSolidProg.aNormal, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          OFFSET_OF(Vertex, normal));
  glEnableVertexAttribArray(mLocSolidProg.aTextureCoord);
  glVertexAttribPointer(mLocSolidProg.aTextureCoord, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          OFFSET_OF(Vertex, textCoords));
  // Now, the indices
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
               indices.data(), GL_STATIC_DRAW);
  // Unbind the vao we will use it for render
  glBindVertexArray(0);
  // Now that we have the data in the GPU and the reference in vao we do not need to keep it
  glDeleteBuffers(1, &vertexBuffer);
  glDeleteBuffers(1, &indexBuffer);
  vertices.clear();
  indices.clear();
}

void SoftBodyApp::create_fabric_solid_model() {
  using namespace mesh;
  // Create model
  Mesh fabricMesh = mBody.getFabricSolidMesh();
  mNumIndicesSolidFabric = fabricMesh.indicesCount();
  // Query data
  std::vector<unsigned int> indices = fabricMesh.getIndices();
  mFabricSolidVertices = fabricMesh.getVertices();
  // Load textures and send them to GPU
  mFabricDiffuseMap = image::Texture("models/fabric.png");
  mFabricDiffuseMap.send_to_gpu();
  mFabricSpecularMap = image::Texture("models/fabric_specular.png");
  mFabricSpecularMap.send_to_gpu();
  // Create the vertex buffer objects and VAO
  //GLuint vertexBuffer;
  GLuint indexBuffer;
  glGenVertexArrays(1, &mVaoFabricSolid);
  glGenBuffers(1, &mFabricSolidVerticesVBO);
  glGenBuffers(1, &indexBuffer);
  // Bind the vao this need to be done before anything
  glBindVertexArray(mVaoFabricSolid);
  // Send data to GPU: first send the vertices
  glBindBuffer(GL_ARRAY_BUFFER, mFabricSolidVerticesVBO);
  glBufferData(GL_ARRAY_BUFFER, mFabricSolidVertices.size() * sizeof(Vertex), mFabricSolidVertices.data(), GL_DYNAMIC_DRAW);
  glEnableVertexAttribArray(mLocSolidProg.aPosition);
  glVertexAttribPointer(mLocSolidProg.aPosition, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        OFFSET_OF(Vertex, position));
  glEnableVertexAttribArray(mLocSolidProg.aNormal);
  glVertexAttribPointer(mLocSolidProg.aNormal, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          OFFSET_OF(Vertex, normal));
  glEnableVertexAttribArray(mLocSolidProg.aTextureCoord);
  glVertexAttribPointer(mLocSolidProg.aTextureCoord, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          OFFSET_OF(Vertex, textCoords));
  // Now, the indices
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
               indices.data(), GL_STATIC_DRAW);
  // Unbind the vao we will use it for render
  glBindVertexArray(0);
  // Now that we have the data in the GPU and the reference in vao we do not need to keep it
  glDeleteBuffers(1, &indexBuffer);
  indices.clear();
}

void SoftBodyApp::create_fabric_wire_model() {
  using namespace mesh;
  // Create model
  Mesh fabricMesh = mBody.getFabricWireMesh();
  mNumIndicesWireFabric = fabricMesh.indicesCount();
  // Query data
  std::vector<unsigned int> indices = fabricMesh.getIndices();
  mFabricWireVertices = fabricMesh.getVertices();
  // Create the vertex buffer objects and VAO
  GLuint indexBuffer;
  glGenVertexArrays(1, &mVaoFabricWire);
  glGenBuffers(1, &mFabricWireVerticesVBO);
  glGenBuffers(1, &indexBuffer);
  // Bind the vao this need to be done before anything
  glBindVertexArray(mVaoFabricWire);
  // Send data to GPU: first send the vertices
  glBindBuffer(GL_ARRAY_BUFFER, mFabricWireVerticesVBO);
  glBufferData(GL_ARRAY_BUFFER, mFabricWireVertices.size() * sizeof(Vertex), mFabricWireVertices.data(), GL_DYNAMIC_DRAW);
  glEnableVertexAttribArray(mLocWireProg.aPosition);
  glVertexAttribPointer(mLocWireProg.aPosition, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
						OFFSET_OF(Vertex, position));
  glEnableVertexAttribArray(mLocWireProg.aForce);
  glVertexAttribPointer(mLocWireProg.aForce, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
						  OFFSET_OF(Vertex, normal));
  // Now, the indices
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
			   indices.data(), GL_STATIC_DRAW);
  // Unbind the vao we will use it for render
  glBindVertexArray(0);
  // Now that we have the data in the GPU and the reference in vao we do not need to keep it
  glDeleteBuffers(1, &indexBuffer);
  indices.clear();
}

void SoftBodyApp::render() {
  // Calculate  camera parameters
  // Identity matrix, as start for some calculations
  glm::mat4 I(1.0f);
  // View
  glm::vec3 camera_up = glm::vec3(0.0f, 1.0f, 0.0f);
  glm::vec3 camera_position = glm::vec3(0.0f, 1.25f, 5.0f);
  glm::vec3 camera_eye = glm::vec3(0.0f, 0.75f, 0.0f);
  glm::mat4 Cam_Init_Pos = glm::lookAt(camera_position, camera_eye, camera_up);
  // Use trackball to get a rotation applied to the camera's initial pose
  glm::mat4 V = Cam_Init_Pos * mTrackBall.getRotation();
  // Projection
  int width;
  int height;
  glfwGetWindowSize(mWinPtr, &width, &height);
  GLfloat aspect = float(width) / float(height);
  const float TAU = 6.28318f; // Math constant equal two PI (Remember, we are in radians)
  GLfloat fovy = TAU / 8.0f + mZoomLevel * (TAU / 50.0f);
  GLfloat zNear = 1.0f;
  GLfloat zFar = 20.0f;
  glm::mat4 P = glm::perspective(fovy, aspect, zNear, zFar);
  // Prepare for render
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Draw the objects in the scene
  if (mRenderMode == 1) {
	mGLProgramSolidPtr->use();
	render_box_solid(P, V);
	render_fabric_solid(P, V);
	render_floor(P, V);
	render_ball(P, V);
  } else {
	mGLProgramSolidPtr->use();
	render_floor(P, V);
	render_ball(P, V);
	mGLProgWireFramePtr->use();
	render_box_wire(P, V);
	render_fabric_wire(P, V);
  }

  // Clean the state for other render (could be the UI, other rendering pass, or the next frame)
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, 0);
  glBindVertexArray(0);
  glUseProgram(0);
}

void SoftBodyApp::render_box_solid(const glm::mat4& P, const glm::mat4& V) {
  // Bind the VAO for thes model
  glBindVertexArray(mVaoBoxSolid);
  // Bind textures for this model
  // Send diffuse texture in unit 0
  glActiveTexture(GL_TEXTURE0);
  mBoxDiffuseMap.bind();
  glUniform1i(mLocSolidProg.uDiffuseMap, 0);
  // Send specular texture in unit 1
  glActiveTexture(GL_TEXTURE1);
  mBoxSpecularMap.bind();
  glUniform1i(mLocSolidProg.uSpecularMap, 1);
  // Identity matrix, as start for calculations
  glm::mat4 I(1.0f);
  // Calculate model matrix
  const float bias = 0.001f;
  glm::mat4 M = glm::translate(I, glm::vec3(0.0f, bias, 0.0f));
  // Send uniform values to shader
  if (mLocSolidProg.uAlpha != -1) {
    glUniform1f(mLocSolidProg.uAlpha, 4.0f);
  }
  if (mLocSolidProg.uPVM != -1) {
    glUniformMatrix4fv(mLocSolidProg.uPVM, 1, GL_FALSE, glm::value_ptr(P * V * M));
  }
  if (mLocSolidProg.uNormalMat != -1) {
    glUniformMatrix4fv(mLocSolidProg.uNormalMat, 1, GL_FALSE,
        glm::value_ptr(glm::inverse(glm::transpose(V * M))));
  }
  // Draw SoftBody's box
  const int startIndex = 0;
  const int startVertex = 0;
  glDrawElementsBaseVertex(GL_TRIANGLES, mNumIndicesSolidBox, GL_UNSIGNED_INT,
                           reinterpret_cast<void*>(startIndex * int(sizeof(unsigned int))),
                           startVertex);
}

void SoftBodyApp::render_box_wire(const glm::mat4& P, const glm::mat4& V) {
  // Bind the VAO for this model
  glBindVertexArray(mVaoBoxWire);
  // Identity matrix, as start for calculations
  glm::mat4 I(1.0f);
  // Calculate model matrix
  const float bias = 0.001f;
  glm::mat4 M = glm::translate(I, glm::vec3(0.0f, bias, 0.0f));
  // Send uniform values to shader
  if (mLocWireProg.uPVM != -1) {
	glUniformMatrix4fv(mLocWireProg.uPVM, 1, GL_FALSE, glm::value_ptr(P * V * M));
  }
  if (mLocWireProg.uMaxForce != -1) {
    // This is a dummy value. All the forces for the box are zero
    // I need to put it since I use the same shaders as the wire fabric
	glUniform1f(mLocWireProg.uMaxForce, 1.0f);
  }
  if (mLocWireProg.uHighForceColor != -1) {
	// This is a dummy value. All the forces for the box are zero
    // I need to put it since I use the same shaders as the wire fabric
	const glm::vec3 black = glm::vec3(0.0f);
	glUniform3fv(mLocWireProg.uHighForceColor, 1, glm::value_ptr(black));
  }
  if (mLocWireProg.uLowForceColor != -1) {
	// Since all the forces in the box are zero this will be the box's color
  	const glm::vec3 blue = glm::vec3(0.0f, 0.0f, 1.0f);
  	glUniform3fv(mLocWireProg.uLowForceColor, 1, glm::value_ptr(blue));
  }
  // Draw SoftBody's box
  const int startIndex = 0;
  const int startVertex = 0;
  glDrawElementsBaseVertex(GL_LINES, mNumIndicesWireBox, GL_UNSIGNED_INT,
						   reinterpret_cast<void*>(startIndex * int(sizeof(unsigned int))),
						   startVertex);
}

void SoftBodyApp::render_ball(const glm::mat4& P, const glm::mat4& V) {
  glBindVertexArray(mVaoBall);
  // Send diffuse texture in unit 0
  glActiveTexture(GL_TEXTURE0);
  mBallDiffuseMap.bind();
  glUniform1i(mLocSolidProg.uDiffuseMap, 0);
  // Send specular texture in unit 1
  glActiveTexture(GL_TEXTURE1);
  mBallSpecularMap.bind();
  glUniform1i(mLocSolidProg.uSpecularMap, 1);
  // Identity matrix, as start for calculations
  glm::mat4 I(1.0f);
  // Calculate model matrix
  glm::mat4 M = glm::translate(I, mBall.getPosition());
  M = glm::scale(M, mBall.getRadious() * glm::vec3(1.0f));
  // Send uniform values to shader
  if (mLocSolidProg.uPVM != -1) {
    glUniformMatrix4fv(mLocSolidProg.uPVM, 1, GL_FALSE, glm::value_ptr(P * V * M));
  }
  if (mLocSolidProg.uNormalMat != -1) {
    glUniformMatrix4fv(mLocSolidProg.uNormalMat, 1, GL_FALSE,
        glm::value_ptr(glm::inverse(glm::transpose(V * M))));
  }
  if (mLocSolidProg.uAlpha != -1) {
    glUniform1f(mLocSolidProg.uAlpha, 8.0f);
  }
  // Draw Ball
  const int startIndex = 0;
  const int startVertex = 0;
  glDrawElementsBaseVertex(GL_TRIANGLES, mNumIndicesBall, GL_UNSIGNED_INT,
                           reinterpret_cast<void*>(startIndex * int(sizeof(unsigned int))),
                           startVertex);
}

void SoftBodyApp::render_floor(const glm::mat4& P, const glm::mat4& V) {
  // Bind the VAO for this model
  glBindVertexArray(mVaoFloor);
  // Bind textures for this model
  // Send diffuse texture in unit 0
  glActiveTexture(GL_TEXTURE0);
  mGroundDiffuseMap.bind();
  glUniform1i(mLocSolidProg.uDiffuseMap, 0);
  // Send specular texture in unit 1
  glActiveTexture(GL_TEXTURE1);
  mGroundSpecularMap.bind();
  glUniform1i(mLocSolidProg.uSpecularMap, 1);
  // Identity matrix, as start for calculations
  glm::mat4 I(1.0f);
  // Calculate model matrix
  glm::mat4 M = glm::rotate(I, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
  M = glm::scale(M, glm::vec3(50.0f, 50.0f, 1.0f));
  // Send uniform values to shader
  if (mLocSolidProg.uPVM != -1) {
    glUniformMatrix4fv(mLocSolidProg.uPVM, 1, GL_FALSE, glm::value_ptr(P * V * M));
  }
  if (mLocSolidProg.uNormalMat != -1) {
    glUniformMatrix4fv(mLocSolidProg.uNormalMat, 1, GL_FALSE,
        glm::value_ptr(glm::inverse(glm::transpose(V * M))));
  }
  if (mLocSolidProg.uAlpha != -1) {
    glUniform1f(mLocSolidProg.uAlpha, 128.0f);
  }
  // Draw Floor
  const int startIndex = 0;
  const int startVertex = 0;
  glDrawElementsBaseVertex(GL_TRIANGLES, mNumIndicesFloor, GL_UNSIGNED_INT,
                           reinterpret_cast<void*>(startIndex * int(sizeof(unsigned int))),
                           startVertex);
}

void SoftBodyApp::render_fabric_solid(const glm::mat4& P, const glm::mat4& V) {
  // Bind the VAO for this model
  glBindVertexArray(mVaoFabricSolid);
  // Bind textures for this model
  // Send diffuse texture in unit 0
  glActiveTexture(GL_TEXTURE0);
  mFabricDiffuseMap.bind();
  glUniform1i(mLocSolidProg.uDiffuseMap, 0);
  // Send specular texture in unit 1
  glActiveTexture(GL_TEXTURE1);
  mFabricSpecularMap.bind();
  glUniform1i(mLocSolidProg.uSpecularMap, 1);
  // Identity matrix, as start for calculations
  glm::mat4 I(1.0f);
  // Calculate model matrix
  glm::mat4 M = I;
  // Send uniform values to shader
  if (mLocSolidProg.uPVM != -1) {
    glUniformMatrix4fv(mLocSolidProg.uPVM, 1, GL_FALSE, glm::value_ptr(P * V * M));
  }
  if (mLocSolidProg.uNormalMat != -1) {
    glUniformMatrix4fv(mLocSolidProg.uNormalMat, 1, GL_FALSE,
        glm::value_ptr(glm::inverse(glm::transpose(V * M))));
  }
  if (mLocSolidProg.uAlpha != -1) {
    glUniform1f(mLocSolidProg.uAlpha, 256.0f);
  }
  // Draw Fabric
  const int startIndex = 0;
  const int startVertex = 0;
  glDrawElementsBaseVertex(GL_TRIANGLES, mNumIndicesSolidFabric, GL_UNSIGNED_INT,
                           reinterpret_cast<void*>(startIndex * int(sizeof(unsigned int))),
                           startVertex);
}


void SoftBodyApp::render_fabric_wire(const glm::mat4& P, const glm::mat4& V) {
  // Bind the VAO for this model
  glBindVertexArray(mVaoFabricWire);
  // Identity matrix, as start for calculations
  glm::mat4 I(1.0f);
  // Calculate model matrix
  glm::mat4 M = I;
  // Send uniform values to shader
  if (mLocSolidProg.uPVM != -1) {
	glUniformMatrix4fv(mLocSolidProg.uPVM, 1, GL_FALSE, glm::value_ptr(P * V * M));
  }
  if (mLocWireProg.uMaxForce != -1) {
	glUniform1f(mLocWireProg.uMaxForce, mMaxForceRange);
  }
  if (mLocWireProg.uHighForceColor != -1) {
	const glm::vec3 red = glm::vec3(1.0f, 0.0f, 0.0f);
	glUniform3fv(mLocWireProg.uHighForceColor, 1, glm::value_ptr(red));
  }
  if (mLocWireProg.uLowForceColor != -1) {
	const glm::vec3 yellow = glm::vec3(1.0f, 1.0f, 0.0f);
	glUniform3fv(mLocWireProg.uLowForceColor, 1, glm::value_ptr(yellow));
  }
  // Draw Fabric in two passes
  // First draw the points
  int startIndex = 0;
  const int startVertex = 0;
  size_t num_points = mFabricWireVertices.size();
  glDrawElementsBaseVertex(GL_POINTS, num_points, GL_UNSIGNED_INT,
						   reinterpret_cast<void*>(startIndex * int(sizeof(unsigned int))),
						   startVertex);
  // Now draw the lines
  // The way we constructed our mesh it has the indices that define the lines
  // after the indices that the define the points
  startIndex = num_points;
  int num_indices_lines = mNumIndicesWireFabric - num_points;
  glDrawElementsBaseVertex(GL_LINES, num_indices_lines, GL_UNSIGNED_INT,
  						   reinterpret_cast<void*>(startIndex * int(sizeof(unsigned int))),
  						   startVertex);

}

void SoftBodyApp::update() {
  // We use GLFW (rather than OpenGL) to timer
  double time = glfwGetTime();
  double elapsed = time - mLastTime; // elapsed is time in seconds between frames
  // mBody.update(float(elapsed));
  mElapsed = float(elapsed);
  mStats.integrateTime.tick();
  if (mFall && mAnimation) {
	mBall.update(0.0025f, mBody.mParams.G);
  }
  if (mAnimation) {
	mBody.update(0.0025f);
  }
  mStats.integrateTime.tock();
  mStats.plotData[1] = mStats.integrateTime.getDeltaMiliS();

  mStats.colisionsTime.tick();
  if (mFall && mAnimation) {
	mBall.solveCollisionFloor(0.0f);
  }
  if (mAnimation) {
	mBody.solveCollisionFloor(0.0f);
	mBody.solveCollisionSphere(mBall);
  }
  mStats.colisionsTime.tock();
  mStats.plotData[2] = mStats.colisionsTime.getDeltaMiliS();

  mStats.remeshTime.tick();
  update_fabric_mesh();
  mStats.remeshTime.tock();
  mStats.plotData[3] = mStats.remeshTime.getDeltaMiliS();
  mLastTime = time;
}

void SoftBodyApp::update_fabric_mesh() {
  using namespace mesh;
  // Ask SoftBody for new positions and normals
  mBody.updateFabricSolidVertices(mFabricSolidVertices);
  // Bind the vao this need to be done before anything
  glBindVertexArray(mVaoFabricSolid);
  // Send data to GPU: first send the vertices
  glBindBuffer(GL_ARRAY_BUFFER, mFabricSolidVerticesVBO);
  glBufferData(GL_ARRAY_BUFFER, mFabricSolidVertices.size() * sizeof(Vertex), mFabricSolidVertices.data(), GL_DYNAMIC_DRAW);
  glEnableVertexAttribArray(mLocSolidProg.aPosition);
  glVertexAttribPointer(mLocSolidProg.aPosition, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        OFFSET_OF(Vertex, position));
  glEnableVertexAttribArray(mLocSolidProg.aNormal);
  glVertexAttribPointer(mLocSolidProg.aNormal, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          OFFSET_OF(Vertex, normal));
  glEnableVertexAttribArray(mLocSolidProg.aTextureCoord);
  glVertexAttribPointer(mLocSolidProg.aTextureCoord, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          OFFSET_OF(Vertex, textCoords));
  // Repeat for wireframe fabric
  // Ask SoftBody for new positions and forces
  mBody.updateFabricWireVertices(mFabricWireVertices);

  // Bind the vao this need to be done before anything
  glBindVertexArray(mVaoFabricWire);
  // Send data to GPU: first send the vertices
  glBindBuffer(GL_ARRAY_BUFFER, mFabricWireVerticesVBO);
  glBufferData(GL_ARRAY_BUFFER, mFabricWireVertices.size() * sizeof(Vertex), mFabricWireVertices.data(), GL_DYNAMIC_DRAW);
  glEnableVertexAttribArray(mLocWireProg.aPosition);
  glVertexAttribPointer(mLocWireProg.aPosition, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
					  OFFSET_OF(Vertex, position));
  glEnableVertexAttribArray(mLocWireProg.aForce);
  glVertexAttribPointer(mLocWireProg.aForce, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
						OFFSET_OF(Vertex, normal));
  // Unbind the vao we will use it for render
  glBindVertexArray(0);
}

void SoftBodyApp::start_logging() {
  mStats.reset();
  for (auto& a : mAccumulatorsFrame) {
	a = 0.0f;
  }
  for (auto& a : mAccumulatorsForce) {
    a = 0.0f;
  }
  mFrameCounter = 0;
  mFileLog.open("logStats.csv", std::ofstream::out | std::ofstream::trunc);
  mFileLog << "Render, Integrate, Collision, Mesh, Frame, Gravity, Spring, Volume, Pressure, Forces" << std::endl;
}

void SoftBodyApp::log_to_file() {
  for (size_t i = 0; i < mStats.plotData.size(); ++i) {
    mFileLog << mStats.plotData[i] << ", ";
  }
  for (size_t i = 0; i < (mBody.mTimers.plotData.size() - 1); i++) {
	mFileLog << mBody.mTimers.plotData[i] << ", ";
  }
  mFileLog << mBody.mTimers.plotData[mBody.mTimers.plotData.size() - 1] << std::endl;
  mFrameCounter++;
}


void SoftBodyApp::free_resources() {
  /* Release imgui resources */
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
  /* Delete OpenGL program */
  delete mGLProgramSolidPtr;
  // Window and context destruction
  glfwDestroyWindow(mWinPtr);
}

void SoftBodyApp::run() {
  // Application's setup
  init_glfw();
  load_OpenGL();
  setup_menu();
  init_program();
  register_glfw_callbacks();
  // For performance tracking
  start_logging();
  // Application's main loop
  while (!glfwWindowShouldClose(mWinPtr)) {
	mStats.frameTime.tick();
	mStats.renderTime.tick();
    if (mShowMenu) {
      create_menu();
      if (mShowDemoMenu) {
        ImGui::ShowDemoWindow();
      }
      ImGui::Render(); // Prepare to render our menu, before clearing buffers (Before scene)
    }
    render();  // Render scene
    // Render the user menu (After scene)
    if (mShowMenu) {
      ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
    mStats.renderTime.tock();
    // to present the data in the menu need to store the result
    // since the clock is invalid between ticks
    //mStats.renderDelta = mStats.renderTime.getDeltaMiliS();
    mStats.plotData[0] = mStats.renderTime.getDeltaMiliS();

    glfwSwapBuffers(mWinPtr);
    update();
    glfwPollEvents();
    mStats.frameTime.tock();
    //mStats.frameDelta = mStats.frameTime.getDeltaMiliS();
    mStats.plotData[4] = mStats.frameTime.getDeltaMiliS();
    log_to_file();
  }
  // Window was closed. Clean and finalize
  mFileLog.close();
  free_resources();
}
