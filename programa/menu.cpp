// Includes from this template
#include "ogl/oglhelpers.h"
// Includes from this project
#include "SoftBodyApp.h"

void SoftBodyApp::setup_menu() {
  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  // Basic look and feel
  ImGui::StyleColorsDark();
  // Use standard connection with GLFW and modern OpenGL
  ImGui_ImplGlfw_InitForOpenGL(mWinPtr, true);
  const char* glsl_version{"#version 130"};
  ImGui_ImplOpenGL3_Init(glsl_version);
}

void SoftBodyApp::create_menu() {
  // Start the Dear ImGui frame
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
  // Draw the ui menu
  ImGui::Begin("Options"); //Name of the menu
    ImGui::Text("Animation");
    if (ImGui::Button(mAnimation ? "Pause" : "Play")) {
      mAnimation = !mAnimation;
    }
    ImGui::SameLine();
    if (ImGui::Button("Reset")) {
      mBody.reset();
      mBall.reset();
      mBall.move(glm::vec3(0.0f, mBallInitialHeight, 0.0f));
    }
    ImGui::Separator();
    ImGui::Text("Spring-Damper"); //Simple text
    ImGui::SliderFloat("Ks", &mBody.mParams.Ks, 0.01f, 150.0f, "%.1f");
    ImGui::SliderFloat("Kd", &mBody.mParams.Kd, 0.0f,  7.0f, "%.2f");
    ImGui::Text("Gas"); //Simple text
    ImGui::Checkbox("Pressure", &mBody.mParams.pressure);
    if (mBody.mParams.pressure) {
      ImGui::SliderFloat("Kg", &mBody.mParams.Kg, 0.0f, 25.0f, "%.1f");
      ImGui::Text("Volume: ");
	  ImGui::SameLine();
	  ImGui::TextColored(ImVec4(0,0.5,1,1), "%.3f", mBody.mSoftBodyVolume);
    }
    ImGui::Checkbox("Gravity", &mBody.mParams.gravity);
    if (mBody.mParams.gravity) {
      ImGui::SliderFloat("G", &mBody.mParams.G, -20.0f, 1.0f, "%.2f");
    }
    ImGui::Text("Sphere"); //Simple text
    ImGui::SliderFloat("Mass", &mBody.mParams.MSphere, 0.1f, 3.0f, "%.2f");
    if (ImGui::Button("Drop")) {
      mFall = !mFall;
    }
    ImGui::Separator();
    ImGui::Text("Integrator"); //Simple text
    if (ImGui::RadioButton("Euler", &mIntegrator, 1)) {
      mBody.setIntegrator(mIntegrator);
      mBall.setIntegrator(mIntegrator);
    }
    if (ImGui::RadioButton("Runge-Kutta", &mIntegrator, 2)) {
      mBody.setIntegrator(mIntegrator);
      mBall.setIntegrator(mIntegrator);
    }
    ImGui::Separator();
    ImGui::Text("Render mode"); //Simple text
    ImGui::RadioButton("Solid", &mRenderMode, 1);
    ImGui::RadioButton("Wireframe", &mRenderMode, 2);

    if (ImGui::CollapsingHeader("Application stats")) {
      ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
      ImGui::Separator();
      std::array<float, 5> plotData;
      for (size_t i = 0; i < mStats.plotData.size(); ++i) {
        plotData[i] = float(mStats.plotData[i]);
        mAccumulatorsFrame[i] += plotData[i];
      }
      float normFrame = mAccumulatorsFrame[4] / mFrameCounter;
      ImGui::TextColored(ImVec4(1,0.5,0,1), "Frame related");
      ImGui::Text("             Avg(ms)    %%");
      ImGui::Text("Render:      %.3f   %04.1f", mAccumulatorsFrame[0] / mFrameCounter,
    		  (mAccumulatorsFrame[0] * 100.0) / (mFrameCounter * normFrame));
      ImGui::Text("Integration: %.3f   %04.1f", mAccumulatorsFrame[1] / mFrameCounter,
    		  (mAccumulatorsFrame[1] * 100.0) / (mFrameCounter * normFrame));
      ImGui::Text("Colisions:   %.3f   %04.1f", mAccumulatorsFrame[2] / mFrameCounter,
    		  (mAccumulatorsFrame[2] * 100.0) / (mFrameCounter * normFrame));
      ImGui::Text("Update geo:  %.3f   %04.1f", mAccumulatorsFrame[3] / mFrameCounter,
    		  (mAccumulatorsFrame[3] * 100.0) / (mFrameCounter * normFrame));
      ImGui::Text("Frame:       %.3f", mAccumulatorsFrame[4] / mFrameCounter);
      ImGui::PlotHistogram("ms", plotData.data(), plotData.size(), 0, nullptr, 0.0f, 8.5f, ImVec2(0,80));
      ImGui::Separator();
	  std::array<float, 5> plotDataForces;
	  for (size_t i = 0; i < mBody.mTimers.plotData.size(); ++i) {
	    plotDataForces[i] = float(mBody.mTimers.plotData[i]);
	    mAccumulatorsForce[i] += plotDataForces[i];
	  }
	  float normForce = mAccumulatorsForce[4] / mFrameCounter;
	  ImGui::TextColored(ImVec4(1,0.5,0,1), "Force related");
	  ImGui::Text("               Avg(ms)    %%");
	  ImGui::Text("Gravity:       %.3f   %04.1f", mAccumulatorsForce[0] / mFrameCounter,
			  (mAccumulatorsForce[0] * 100.0) / (mFrameCounter * normForce));
	  ImGui::Text("Spring-Damper: %.3f   %04.1f", mAccumulatorsForce[1] / mFrameCounter,
			  (mAccumulatorsForce[1] * 100.0) / (mFrameCounter * normForce));
	  ImGui::Text("Volume:        %.3f   %04.1f", mAccumulatorsForce[2] / mFrameCounter,
			  (mAccumulatorsForce[2] * 100.0) / (mFrameCounter * normForce));
	  ImGui::Text("Pressure:      %.3f   %04.1f", mAccumulatorsForce[3] / mFrameCounter,
			  (mAccumulatorsForce[3] * 100.0) / (mFrameCounter * normForce));
	  ImGui::Text("Forces:        %.3f", mAccumulatorsForce[4] / mFrameCounter);
	  ImGui::PlotHistogram("Frame", plotDataForces.data(), plotDataForces.size(), 0, nullptr, 0.0f, 4.5f, ImVec2(0,80));
	  ImGui::Separator();
      ImGui::Text("OpenGL's debug log is %s", mHasDebug ? "enabled" : "disabled");
    }
    //Imgui's controls return true on interaction
	if (ImGui::CollapsingHeader("Environment info:")) { // Submenu
	  ImGui::Text("%s", "Hardware");
	  ImGui::TextColored(ImVec4(0,0.5,1,1), "GPU:");
	  ImGui::SameLine();
	  ImGui::Text("%s", ogl::get_version(ogl::GPU).c_str());
	  ImGui::TextColored(ImVec4(0,0.5,1,1), "Vendor:");
	  ImGui::SameLine();
	  ImGui::Text("%s", ogl::get_version(ogl::VENDOR).c_str());
	  ImGui::Separator();
	  ImGui::Text("%s", "Software");
	  ImGui::TextColored(ImVec4(0,0.5,1,1), "OpenGL:");
	  ImGui::SameLine();
	  ImGui::Text("%s", ogl::get_version(ogl::OPENGL).c_str());
	  ImGui::TextColored(ImVec4(0,0.5,1,1), "GLSL:");
	  ImGui::SameLine();
	  ImGui::Text("%s", ogl::get_version(ogl::GLSL).c_str());
	  ImGui::Separator();
	  ImGui::TextColored(ImVec4(1,0.5,0,1), "Libraries");
	  ImGui::TextColored(ImVec4(0,0.5,1,1), "GLFW:");
	  ImGui::SameLine();
	  ImGui::Text("%s", ogl::get_version(ogl::GLFW).c_str());
	  ImGui::TextColored(ImVec4(0,0.5,1,1), "Dear ImGui:");
	  ImGui::SameLine();
	  ImGui::Text("%s", ogl::get_version(ogl::IMGUI).c_str());
	  ImGui::TextColored(ImVec4(0,0.5,1,1), "GLM:");
	  ImGui::SameLine();
	  ImGui::Text("%s", ogl::get_version(ogl::GLM).c_str());
	  ImGui::TextColored(ImVec4(0,0.5,1,1), "GLEW:");
	  ImGui::SameLine();
	  ImGui::Text("%s", ogl::get_version(ogl::GLEW).c_str());
	  ImGui::TextColored(ImVec4(0,0.5,1,1), "Assimp:");
	  ImGui::SameLine();
	  ImGui::Text("%s", ogl::get_version(ogl::ASSIMP).c_str());
	  ImGui::TextColored(ImVec4(0,0.5,1,1), "FreeImage:");
	  ImGui::SameLine();
	  ImGui::Text("%s", ogl::get_version(ogl::FREEIMAGE).c_str());
	}
  ImGui::End();
}
