/**
 * @file		engine.h
 * @brief	Simple 3D engine main include
 *
 * @author	Achille Peternier (achille.peternier@supsi.ch), (C) SUPSI
 */
#pragma once



//////////////
// #INCLUDE //
//////////////
   
   // C/C++:    
   #include <iostream>
   #include <string>
   #include <vector>
   #include <list>   
   #include <memory> 

   // GLM:
#ifndef _DEBUG
   #define GLM_FORCE_INLINE
   #define GLM_FORCE_SSE4
#endif
   #include <glm/glm.hpp>   
   #include <glm/detail/type_half.hpp>
   #include <glm/gtc/matrix_transform.hpp>
   #include <glm/gtc/matrix_inverse.hpp>
   #include <glm/gtc/type_ptr.hpp>
   #include <glm/gtx/string_cast.hpp>   
   #include <glm/gtx/quaternion.hpp>   
   #include <glm/gtc/packing.hpp>  



/////////////
// VERSION //
/////////////

   // Export API:
#ifdef _WINDOWS
   // Specifies i/o linkage (VC++ spec):
   #ifdef ENGINE_EXPORTS
      #define ENG_API __declspec(dllexport)
   #else
      #define ENG_API __declspec(dllimport)
   #endif      

   // Get rid of annoying warnings:
   #pragma warning(disable : 4251) 
#else // Under linux
   #define ENG_API
   #define __stdcall
#endif



///////////////
// NAMESPACE //
///////////////

namespace Eng {



//////////////
// #INCLUDE //
//////////////   

   // Logging:
   #include "engine_log.h"

   // Architecture:
   #include "engine_object.h"
   #include "engine_managed.h"

   // File formats:
   #include "engine_serializer.h"
   #include "engine_bitmap.h"
   #include "engine_ovo.h"

   // Objects:
   #include "engine_vao.h"
   #include "engine_vbo.h"
   #include "engine_ebo.h"
   #include "engine_shader.h"
   #include "engine_program.h"
   #include "engine_texture.h"
   #include "engine_material.h"
   #include "engine_fbo.h"

   // Scene-graph elems:
   #include "engine_node.h"
   #include "engine_mesh.h"
   #include "engine_light.h"
   #include "engine_camera.h"
   #include "engine_list.h"

   // Storage:
   #include "engine_container.h"

   // Pipelines:
   #include "engine_pipeline.h"
   #include "engine_pipeline_shadowmapping.h"
   #include "engine_pipeline_fullscreen2d.h"
   #include "engine_pipeline_default.h"
   


///////////////////////
// MAIN ENGINE CLASS //
///////////////////////
   
/**
 * @brief Base engine main class. This class is a singleton.
 */
class ENG_API Base final
{
//////////
public: //
//////////	   

   // Special values:
   constexpr static int32_t dfltWindowSizeX = 1024;     ///< Default window width
   constexpr static int32_t dfltWindowSizeY = 768;      ///< Default window height

   // Callback signatures:
   typedef void (* KeyboardCallback)   (int key, int scancode, int action, int mods);
   typedef void (* MouseCursorCallback)(double mouseX, double mouseY);
   typedef void (* MouseButtonCallback)(int button, int action, int mods);
   typedef void (* MouseScrollCallback)(double scrollX, double scrollY);

   // Const/dest:
   Base(Base const &) = delete;
   ~Base();

   // Operators:
   void operator=(Base const &) = delete;

   // Singleton:
   static Base &getInstance();

   // Init/free:
   bool init();
   bool free();

   // Get/set:
   uint64_t getFrameNr() const;
   glm::ivec2 getWindowSize() const;

   // Management:
   bool processEvents();
   bool clear();
   bool swap();
   bool setKeyboardCallback(KeyboardCallback cb);
   bool setMouseCursorCallback(MouseCursorCallback cb);
   bool setMouseButtonCallback(MouseButtonCallback cb);
   bool setMouseScrollCallback(MouseScrollCallback cb);

   // Compatibility:
   bool isBindlessSupported() const;


///////////
private: //
///////////	

   // Reserved:
   struct Reserved;
   std::unique_ptr<Reserved> reserved;

   // Const/dest:
   Base();
};

}; // end of namespace Eng::
