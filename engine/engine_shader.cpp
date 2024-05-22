/**
 * @file		engine_shader.cpp
 * @brief	Shader object
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



////////////
// STATIC //
////////////

   // Special values:
   Eng::Shader Eng::Shader::empty("[empty]");



/////////////////////////
// RESERVED STRUCTURES //
/////////////////////////

/**
 * @brief Shader reserved structure.
 */
struct Eng::Shader::Reserved
{
   Type type;           ///< Shader type
   std::string code;    ///< Shader source code
   GLuint oglId;        ///< OpenGL shader ID


   /**
    * Constructor.
    */
   Reserved() : type{ Eng::Shader::Type::none }, oglId{ 0 }
   {}
};



//////////////////////////
// BODY OF CLASS Shader //
//////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Constructor.
 */
ENG_API Eng::Shader::Shader() : reserved(std::make_unique<Eng::Shader::Reserved>())
{
   ENG_LOG_DETAIL("[+]");
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Constructor with name.
 * @param name node name
 */
ENG_API Eng::Shader::Shader(const std::string &name) : Eng::Object(name), reserved(std::make_unique<Eng::Shader::Reserved>())
{
   ENG_LOG_DETAIL("[+]");
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Move constructor.
 */
ENG_API Eng::Shader::Shader(Shader &&other) : Eng::Object(std::move(other)), Eng::Managed(std::move(other)), reserved(std::move(other.reserved))
{
   ENG_LOG_DETAIL("[M]");
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Destructor.
 */
ENG_API Eng::Shader::~Shader()
{
   ENG_LOG_DETAIL("[-]");
   if (reserved) 
      this->free();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Creates an OpenGL shader.
 * @return TF
 */
bool ENG_API Eng::Shader::init()
{
   if (this->Eng::Managed::init() == false)
      return false;   

   // Free shader if stored:
   if (reserved->oglId)
   {
      glDeleteShader(reserved->oglId);
      reserved->oglId = 0;
   }

   // Create it:		        
   GLuint glKind = 0;
   switch (this->getType())
   {
      case Eng::Shader::Type::vertex:              glKind = GL_VERTEX_SHADER;          break;
      case Eng::Shader::Type::tessellation_ctrl:   glKind = GL_TESS_CONTROL_SHADER;    break;
      case Eng::Shader::Type::tessellation_eval:   glKind = GL_TESS_EVALUATION_SHADER; break;
      case Eng::Shader::Type::geometry:            glKind = GL_GEOMETRY_SHADER;        break;
      case Eng::Shader::Type::fragment:            glKind = GL_FRAGMENT_SHADER;        break;
      case Eng::Shader::Type::compute:             glKind = GL_COMPUTE_SHADER;         break;
      default:
         ENG_LOG_ERROR("Invalid shader type");
         return false;
   }

   // Load program:
   reserved->oglId = glCreateShader(glKind);
   if (reserved->oglId == 0)
   {
      ENG_LOG_ERROR("Unable to create shader");
      return false;
   }

   // Done:   
   return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Destroys an OpenGL instance.
 * @return TF
 */
bool ENG_API Eng::Shader::free()
{
   if (this->Eng::Managed::free() == false)
      return false;   

   // Free shader if stored:
   if (reserved->oglId)
   {
      glDeleteShader(reserved->oglId);
      reserved->oglId = 0;
   }

   // Done:   
   return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Get type.
 * @return shader type
 */
const Eng::Shader::Type ENG_API Eng::Shader::getType() const
{
   return reserved->type;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Get source code.
 * @return source code
 */
const std::string ENG_API &Eng::Shader::getCode() const
{
   return reserved->code;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Return the GLuint shader ID.
 * @return shader ID or 0 if not valid
 */
uint32_t ENG_API Eng::Shader::getOglHandle() const
{
   return reserved->oglId;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Use the specified string as source code for the shader.
 */
bool ENG_API Eng::Shader::load(Type type, const std::string &code)
{
   // Safety net:
   if (code.empty())
   {
      ENG_LOG_ERROR("Invalid params");
      return false;
   }

   // Init shader:
   reserved->type = type;
   if (this->init() == false)
      return false;

   // Pass params:
   reserved->type = type;
   reserved->code = code;   

   // First shader lines:
   std::string glslVersionHeader = "#version 460 core\n"; 
   if (type == Type::fragment && Eng::Base::getInstance().isBindlessSupported())
   {
      glslVersionHeader += "#extension GL_ARB_bindless_texture : require\n";
      glslVersionHeader += "#define ENG_BINDLESS_SUPPORTED\n";
   }
   
   const char *sources[2] = { glslVersionHeader.c_str(), reserved->code.c_str() };
	glShaderSource(reserved->oglId, 2, sources, nullptr);
	glCompileShader(reserved->oglId);

   // Check status:
   GLint status;
   glGetShaderiv(reserved->oglId, GL_COMPILE_STATUS, &status);
   if (status == GL_FALSE)
   {
	   // Verify shader:
      char buffer[Eng::Shader::maxLogSize];
      int32_t length = 0;
      memset(buffer, 0, Eng::Shader::maxLogSize);

      glGetShaderInfoLog(reserved->oglId, Eng::Shader::maxLogSize, &length, buffer);
	   if (length > 0)
	   {
         ENG_LOG_ERROR("Shader not compiled (%s)", buffer);		   
		   return false;
      }
   }
	else
      ENG_LOG_DEBUG("Shader compiled");

   // Done:
   return true;
}