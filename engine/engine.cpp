/**
 * @file		engine.cpp
 * @brief	Simple 3D engine main file
 *
 * @author	Achille Peternier (achille.peternier@supsi.ch), (C) SUPSI
 */



//////////////
// #INCLUDE //
//////////////

   // Main include:
   #include "engine.h"

   // OGL:      
   #include <GL/glew.h>
   #include <GLFW/glfw3.h>

   // C/C++:
   #include <sstream>   

   

/////////////////////////
// RESERVED STRUCTURES //
/////////////////////////

/**
 * @brief Base class reserved structure.
 */
struct Eng::Base::Reserved
{   
   GLFWwindow *window;                 ///< Window handler
   int32_t windowSizeX, windowSizeY;   ///< Window width and height

   // Some counters:
   int64_t frameCounter;               ///< Total number of rendered frames   

   // Compatibility flags:
   bool bindlessSupportFlag;           ///< When true, the current context supports ARB_bindless_texture

   // Callbacks:
   Eng::Base::KeyboardCallback keyboardCallback;
   Eng::Base::MouseCursorCallback mouseCursorCallback;
   Eng::Base::MouseButtonCallback mouseButtonCallback;
   Eng::Base::MouseScrollCallback mouseScrollCallback;


   /**
    * Constructor
    */
   Reserved() : window{ nullptr }, windowSizeX{ 0 }, windowSizeY{ 0 },
                frameCounter{ 0 }, bindlessSupportFlag{ false },
                keyboardCallback{ nullptr },
                mouseCursorCallback{ nullptr },
                mouseButtonCallback{ nullptr },
                mouseScrollCallback{ nullptr }
   {}
};



////////////
// STATIC //
////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Debug message parser for OpenGL.
 * @return formatted string for logging
 */
static std::string FormatDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, const char *msg)
{
   std::stringstream stringStream;
   std::string sourceString;
   std::string typeString;
   std::string severityString;

   switch (source)
   {
      case GL_DEBUG_SOURCE_API:              sourceString = "API"; break;
      case GL_DEBUG_SOURCE_APPLICATION:      sourceString = "Application"; break;
      case GL_DEBUG_SOURCE_WINDOW_SYSTEM:    sourceString = "Window System"; break;
      case GL_DEBUG_SOURCE_SHADER_COMPILER:  sourceString = "Shader Compiler"; break;
      case GL_DEBUG_SOURCE_THIRD_PARTY:      sourceString = "Third Party"; break;
      case GL_DEBUG_SOURCE_OTHER:            sourceString = "Other"; break;
      default:                               sourceString = "Unknown";
   }

   switch (type)
   {
      case GL_DEBUG_TYPE_ERROR:                 typeString = "Error"; break;
      case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:   typeString = "Deprecated Behavior"; break;
      case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:    typeString = "Undefined Behavior"; break;
      case GL_DEBUG_TYPE_PERFORMANCE:           typeString = "Performance"; break;
      case GL_DEBUG_TYPE_OTHER:                 typeString = "Other"; break;
      default:                                  typeString = "Unknown";
   }

   switch (severity)
   {
      case GL_DEBUG_SEVERITY_HIGH:     severityString = "High"; break;
      case GL_DEBUG_SEVERITY_MEDIUM:   severityString = "Medium"; break;
      case GL_DEBUG_SEVERITY_LOW:      severityString = "Low"; break;
      default:                         severityString = "Unknown";
   }

   stringStream << "[OGL] " << msg;
   stringStream << " [Source = " << sourceString;
   stringStream << ", Type = " << typeString;
   stringStream << ", Severity = " << severityString;
   stringStream << ", ID = " << id << "]";

   // Done:
   return stringStream.str();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Debug message callback for OpenGL.
 */
static void __stdcall DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, GLvoid *userParam)
{
   std::string error = FormatDebugOutput(source, type, id, severity, message);
   if (type == GL_DEBUG_TYPE_ERROR)
   {
      ENG_LOG_ERROR("%s", error.c_str());
   }
   else
   {
      ENG_LOG_INFO("%s", error.c_str());
   }
}



////////////////////////
// BODY OF CLASS Base //
////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Constructor.
 */
ENG_API Eng::Base::Base() : reserved(std::make_unique<Eng::Base::Reserved>())
{
   ENG_LOG_DETAIL("[+]");   
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Destructor.
 */
ENG_API Eng::Base::~Base()
{
   ENG_LOG_DETAIL("[-]");   
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Get singleton instance.
 */
Eng::Base ENG_API &Eng::Base::getInstance()
{
   static Base instance;
   return instance;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Init internal components.
 * @param config optional configuration
 * @return TF
 */
bool ENG_API Eng::Base::init()
{  
   /////////////
   // Init glfw:
   typedef void(* GLWF_ERROR_CALLBACK_PTR)(int32_t error, const char *description);
   glfwSetErrorCallback(static_cast<GLWF_ERROR_CALLBACK_PTR>
   (
      // Callback:
      [](int32_t error, const char *description)
      {
         ENG_LOG_ERROR("[GLFW] code: %d, %s", error, description);
      }
   ));

   // Init framework:
   if (!glfwInit())
   {
      ENG_LOG_ERROR("Unable to init GLFW");
      return false;
   }

   int32_t glfwMajor, glfwMinor, glfwRev;
   glfwGetVersion(&glfwMajor, &glfwMinor, &glfwRev);
   ENG_LOG_PLAIN("   Using GLFW v%d.%d.%d", glfwMajor, glfwMinor, glfwRev);

   // Open an OpenGL window:      
   glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
   glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
   glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
   glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_NATIVE_CONTEXT_API);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
   glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef _DEBUG
   glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#else
   glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_FALSE);
   glfwWindowHint(GLFW_CONTEXT_NO_ERROR, GLFW_TRUE);
#endif
   glfwWindowHint(GLFW_RED_BITS, 8);
   glfwWindowHint(GLFW_BLUE_BITS, 8);
   glfwWindowHint(GLFW_GREEN_BITS, 8);
   glfwWindowHint(GLFW_ALPHA_BITS, 8);
   glfwWindowHint(GLFW_DEPTH_BITS, 24);
   glfwWindowHint(GLFW_STENCIL_BITS, 8);

   reserved->window = glfwCreateWindow(Eng::Base::dfltWindowSizeX, Eng::Base::dfltWindowSizeY,
                                       "demo",
                                       nullptr,
                                       nullptr);
   if (reserved->window == nullptr)
   {
      ENG_LOG_ERROR("Unable to create window");
      return false;
   }
   glfwSetWindowUserPointer(reserved->window, this);

   // Set context:
   glfwMakeContextCurrent(reserved->window);

   // Glew:   
   GLenum err = glewInit();
   if (GLEW_OK != err)
   {
      ENG_LOG_ERROR("Unable to init GLEW");
      return false;;
   }
   ENG_LOG_PLAIN("   Using GLEW v%s.%s.%s", glewGetString(GLEW_VERSION_MAJOR), glewGetString(GLEW_VERSION_MINOR), glewGetString(GLEW_VERSION_MICRO));
   if (!glewIsSupported("GL_VERSION_4_6"))
   {
      ENG_LOG_ERROR("OpenGL 4.6 not supported");
      return false;
   }

   // Log and validate supported settings:
   ENG_LOG_PLAIN("OpenGL properties:");
   ENG_LOG_PLAIN("   Vendor  . . . :  %s", glGetString(GL_VENDOR));
   ENG_LOG_PLAIN("   Driver  . . . :  %s", glGetString(GL_RENDERER));

   int32_t oglVersion[2];
   glGetIntegerv(GL_MAJOR_VERSION, &oglVersion[0]);
   glGetIntegerv(GL_MINOR_VERSION, &oglVersion[1]);
   ENG_LOG_PLAIN("   Version . . . :  %s [%d.%d]", glGetString(GL_VERSION), oglVersion[0], oglVersion[1]);
   if (glfwGetWindowAttrib(reserved->window, GLFW_CONTEXT_NO_ERROR))
      ENG_LOG_PLAIN("   No error  . . :  enabled");
   else
      ENG_LOG_PLAIN("   No error  . . :  disabled");      
   ENG_LOG_PLAIN("   GLSL  . . . . :  %s", glGetString(GL_SHADING_LANGUAGE_VERSION));

   int32_t nrOfExtensions;
   glGetIntegerv(GL_NUM_EXTENSIONS, &nrOfExtensions);
   ENG_LOG_PLAIN("   Extensions  . :  %d", nrOfExtensions);

   // Check for required extensions:
   if (!glewIsSupported("GL_ARB_bindless_texture"))
      ENG_LOG_WARN("GL_ARB_bindless_texture not supported: using fallback");
   else
   {  
      ENG_LOG_PLAIN("   Bindless  . . :  supported");
      reserved->bindlessSupportFlag = true;
   }

   if (!glewIsSupported("GL_EXT_texture_compression_s3tc"))
   {
      ENG_LOG_ERROR("GL_EXT_texture_compression_s3tc not supported");
      return false;
   }

   int workGroupSizes[3] = { 0 };
   glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &workGroupSizes[0]);
   glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &workGroupSizes[1]);
   glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &workGroupSizes[2]);
   int workGroupCounts[3] = { 0 };
   glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &workGroupCounts[0]);
   glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &workGroupCounts[1]);
   glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &workGroupCounts[2]);

   ENG_LOG_PLAIN("   Max group sz  :  %d, %d, %d", workGroupSizes[0], workGroupSizes[1], workGroupSizes[2]);
   ENG_LOG_PLAIN("   Max group cnt :  %d, %d, %d", workGroupCounts[0], workGroupCounts[1], workGroupCounts[2]);

   float maxAnisotropy = 0.0f;
   glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &maxAnisotropy);
   ENG_LOG_PLAIN("   Max anisotropy:  %.1f", maxAnisotropy);
   if (maxAnisotropy < 16.0f)
   {
      ENG_LOG_ERROR("Anistropic filter level 16 or higher not supported");
      return false;
   }

#if _DEBUG
   // Query the OpenGL function to register your callback function:
   PFNGLDEBUGMESSAGECALLBACKPROC _glDebugMessageCallback = (PFNGLDEBUGMESSAGECALLBACKPROC) glfwGetProcAddress("glDebugMessageCallback");

   // Register callback function:
   if (_glDebugMessageCallback != nullptr)
   {
      _glDebugMessageCallback((GLDEBUGPROC) DebugCallback, nullptr);
      glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
   }
   else
      ENG_LOG_ERROR("Unable to register debug callback");
#endif   
   glfwGetFramebufferSize(reserved->window, &reserved->windowSizeX, &reserved->windowSizeY);
   glfwSwapInterval(0); // No V-sync
   glViewport(0, 0, reserved->windowSizeX, reserved->windowSizeY);

   // Common OpenGL settings:
   glEnable(GL_DEPTH_TEST);
   glDepthFunc(GL_LEQUAL);
   glPixelStorei(GL_PACK_ALIGNMENT, 1);         // Not sure whether it is really global state
   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);       // Not sure whether it is really global state

   // Done:
   return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Free internal components.
 * @return TF
 */
bool ENG_API Eng::Base::free()
{
   ENG_LOG_DEBUG("Releasing context...");

   // Since the context is about to be released, unload all objects that are still allocated:
   Managed::forceRelease();

   // Release glfw:
   if (reserved->window)
   {
      // Release OGL resources:      
      // ...

      glfwDestroyWindow(reserved->window);
      reserved->window = nullptr;
   }
   glfwTerminate();

   ENG_LOG_PLAIN("   Context deinitialized");

   // Done:
   return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Process events.
 * @return TF
 */
bool ENG_API Eng::Base::processEvents()
{
   glfwPollEvents();

   // Window shall be closed?
   if (glfwWindowShouldClose(reserved->window))
      return false;

   // Done:
   return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Clear buffers.
 * @return TF
 */
bool ENG_API Eng::Base::clear()
{
   glClearColor(1.0f, 0.6f, 0.1f, 1.0f);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   // Done:
   return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Swap buffers.
 * @return TF
 */
bool ENG_API Eng::Base::swap()
{
   // ENG_LOG_DEBUG("Finished with frame %llu", reserved->frameCounter);
   glfwSwapBuffers(reserved->window);

   // New frame:
   reserved->frameCounter++;

   // Done:
   return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Returns the current frame number.
 * @return current frame number
 */
uint64_t ENG_API Eng::Base::getFrameNr() const
{
   return reserved->frameCounter;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Returns the current window size.
 * @return current window size
 */
glm::ivec2 ENG_API Eng::Base::getWindowSize() const
{
   return glm::ivec2(reserved->windowSizeX, reserved->windowSizeY);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Set keyboard callback.
 * @param cb keyboard callback function pointer
 * @return TF
 */
bool ENG_API Eng::Base::setKeyboardCallback(KeyboardCallback cb)
{
   reserved->keyboardCallback = cb;

   // Register callbacks:
   glfwSetKeyCallback(reserved->window, static_cast<GLFWkeyfun>
   (
      // Callback:
      [](GLFWwindow *window, int key, int scancode, int action, int mods)
      {
         Eng::Base *_this = static_cast<Eng::Base *>(glfwGetWindowUserPointer(window));
         _this->reserved->keyboardCallback(key, scancode, action, mods);
      }
   ));

   // Done:
   return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Set mouse cursor callback.
 * @param cb mouse callback function pointer
 * @return TF
 */
bool ENG_API Eng::Base::setMouseCursorCallback(MouseCursorCallback cb)
{
   reserved->mouseCursorCallback = cb;

   // Register callbacks:
   glfwSetCursorPosCallback(reserved->window, static_cast<GLFWcursorposfun>
   (
      // Callback:
      [](GLFWwindow *window, double mouseX, double mouseY)
      {
         Eng::Base *_this = static_cast<Eng::Base *>(glfwGetWindowUserPointer(window));
         _this->reserved->mouseCursorCallback(mouseX, mouseY);
      }
   ));

   // Done:
   return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Set mouse button callback.
 * @param cb mouse button callback function pointer
 * @return TF
 */
bool ENG_API Eng::Base::setMouseButtonCallback(MouseButtonCallback cb)
{
   reserved->mouseButtonCallback = cb;

   // Register callbacks:
   glfwSetMouseButtonCallback(reserved->window, static_cast<GLFWmousebuttonfun>
   (
      // Callback:
      [](GLFWwindow *window, int button, int action, int mods)
      {
         Eng::Base *_this = static_cast<Eng::Base *>(glfwGetWindowUserPointer(window));
         _this->reserved->mouseButtonCallback(button, action, mods);
      }
   ));

   // Done:
   return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Set mouse scroll callback.
 * @param cb mouse scroll callback function pointer
 * @return TF
 */
bool ENG_API Eng::Base::setMouseScrollCallback(MouseScrollCallback cb)
{
   reserved->mouseScrollCallback = cb;

   // Register callbacks:
   glfwSetScrollCallback(reserved->window, static_cast<GLFWscrollfun>
   (
      // Callback:
      [](GLFWwindow *window, double scrollX, double scrollY)
      {
         Eng::Base *_this = static_cast<Eng::Base *>(glfwGetWindowUserPointer(window));
         _this->reserved->mouseScrollCallback(scrollX, scrollY);
      }
   ));

   // Done:
   return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Returns true when ARB_bindless_texture is supported by the current context.
 * @return TF
 */
bool ENG_API Eng::Base::isBindlessSupported() const
{ 
   return reserved->bindlessSupportFlag;
}