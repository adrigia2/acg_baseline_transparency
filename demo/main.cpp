/**
 * @file		main.cpp
 * @brief	Engine usage example
 * 
 * @author	Achille Peternier (achille.peternier@supsi.ch), (C) SUPSI
 */



//////////////
// #INCLUDE //
//////////////

   // Main engine header:
   #include "engine.h"

   // C/C++:
   #include <iostream>
   #include <chrono>

#include "engine_pipeline_OIT.h"


//////////   
// VARS //
//////////

   // Mouse status:
   double oldMouseX, oldMouseY;   
   bool mouseBR, mouseBL;

   // Camera:
   Eng::Camera camera;   

   // Light (loaded from OVO file later):
   std::reference_wrapper<Eng::Light> light = Eng::Light::empty;

   // Pipelines:
   Eng::PipelineDefault dfltPipe;
   Eng::PipelineOIT oitPipe;
   Eng::PipelineFullscreen2D full2dPipe;

   // Flags:
   bool showShadowMap = false;
   bool perspectiveProj = false;



///////////////
// CALLBACKS //
///////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Mouse cursor callback.
 * @param mouseX updated mouse X coordinate
 * @param mouseY updated mouse Y coordinate
 */
void mouseCursorCallback(double mouseX, double mouseY)
{
   // ENG_LOG_DEBUG("x: %.1f, y: %.1f", mouseX, mouseY);
   float deltaX = (float)(mouseY - oldMouseY);
   float deltaY = (float)(mouseX - oldMouseX);
   oldMouseX = mouseX;
   oldMouseY = mouseY;

   // Rotate camera around:
   if (mouseBL)
   {
      camera.rotateAzimuth(deltaY);
      camera.rotatePolar(deltaX);
   }
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Mouse button callback.
 * @param button mouse button id
 * @param action action
 * @param mods modifiers
 */
void mouseButtonCallback(int button, int action, int mods)
{
   // ENG_LOG_DEBUG("button: %d, action: %d, mods: %d", button, action, mods);
   switch (button)
   {
      case 0: mouseBL = (bool) action; break;
      case 1: mouseBR = (bool) action; break;
   }
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Mouse scroll callback.
 * @param scrollX updated mouse scroll X coordinate
 * @param scrollY updated mouse scroll Y coordinate
 */
void mouseScrollCallback(double scrollX, double scrollY)
{
   // ENG_LOG_DEBUG("x: %.1f, y: %.1f", scrollX, scrollY);   
   camera.zoom((float) scrollY);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Keyboard callback.
 * @param key key code
 * @param scancode key scan code
 * @param action action
 * @param mods modifiers
 */
void keyboardCallback(int key, int scancode, int action, int mods)
{
   // ENG_LOG_DEBUG("key: %d, scancode: %d, action: %d, mods: %d", key, scancode, action, mods);
   switch (key)
   {
      case 'W': if (action == 0) oitPipe.setWireframe(!oitPipe.isWireframe()); break;         
      case 'S': if (action == 0) showShadowMap = !showShadowMap; break;
      case 'P': if (action == 0)
                {
                   perspectiveProj = !perspectiveProj;
                   if (perspectiveProj)
                      light.get().setProjMatrix(glm::perspective(glm::radians(75.0f), 1.0f, 1.0f, 1000.0f));    // Perspective projection                     
                   else
                      light.get().setProjMatrix(glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f, 1.0f, 1000.0f));   // Orthographic projection
                }
                break;
   }
}



//////////
// MAIN //
//////////

/**
 * Application entry point.
 * @param argc number of command-line arguments passed
 * @param argv array containing up to argc passed arguments
 * @return error code (0 on success, error code otherwise)
 */
int main(int argc, char *argv[])
{
   // Credits:
   std::cout << "Engine demo, A. Peternier (C) SUPSI" << std::endl;
   std::cout << std::endl;

   // Init engine:
   Eng::Base &eng = Eng::Base::getInstance();
   eng.init();

   // Register callbacks:
   eng.setMouseCursorCallback(mouseCursorCallback);
   eng.setMouseButtonCallback(mouseButtonCallback);
   eng.setMouseScrollCallback(mouseScrollCallback);
   eng.setKeyboardCallback(keyboardCallback);
     

   /////////////////
   // Loading scene:   
   Eng::Ovo ovo; 
   Eng::Node &root = ovo.load("simple3dSceneWithTransp.ovo");
   std::cout << "Scene graph:\n" << root.getTreeAsString() << std::endl;
   
   // Get light ref:
   light = dynamic_cast<Eng::Light &>(Eng::Container::getInstance().find("Omni001"));      
   light.get().setAmbient({ 0.3f, 0.3f, 0.3f });
   light.get().setColor({ 1.5f, 1.5f, 1.5f });
   light.get().setProjMatrix(glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f, 1.0f, 1000.0f)); // Orthographic projection   
   
   // Get torus knot ref:
   Eng::Mesh &tknot = dynamic_cast<Eng::Mesh &>(Eng::Container::getInstance().find("Torus Knot001"));   

   // Rendering elements:
   Eng::List list;      
   
   // Init camera:   
   camera.setProjMatrix(glm::perspective(glm::radians(45.0f), eng.getWindowSize().x / (float) eng.getWindowSize().y, 1.0f, 1000.0f));
   // camera.setProjMatrix(glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, 1.0f, 1000.0f));
   camera.lookAt(root); // Look at the origin

  
   /////////////
   // Main loop:
   std::cout << "Entering main loop..." << std::endl;      
   std::chrono::high_resolution_clock timer;
   float fpsFactor = 0.0f;
   while (eng.processEvents())
   {      
      auto start = timer.now();

      // Update viewpoint:
      camera.update();      

      // Animate torus knot:      
      tknot.setMatrix(glm::rotate(tknot.getMatrix(), glm::radians(15.0f * fpsFactor), glm::vec3(0.0f, 1.0f, 0.0f)));
      
      // Update list:
      list.reset();
      list.process(root);
      
      // Main rendering:
      eng.clear();      
      //dfltPipe.render(camera, list);
         oitPipe.render(camera, list);
         full2dPipe.render(oitPipe.getRenderTexture(), list);
      eng.swap();    

      auto stop = timer.now();
      auto deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count() / 1000.0f;
      float fps = (1.0f / deltaTime) * 1000.0f;
      fpsFactor = 1.0f / fps;
      // std::cout << "fps: " << fps << std::endl;
   }
   std::cout << "Leaving main loop..." << std::endl;

   // Release engine:
   eng.free();

   // Done:
   std::cout << "[application terminated]" << std::endl;
   return 0;
}
