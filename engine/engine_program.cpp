/**
 * @file		engine_program.cpp
 * @brief	OpenGL program object
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
   #include <unordered_map>



////////////
// STATIC //
////////////

   // Special values:
   Eng::Program Eng::Program::empty("[empty]");

   // Cache:
   std::reference_wrapper<Eng::Program> Eng::Program::cache = Eng::Program::empty;



/////////////////////////
// RESERVED STRUCTURES //
/////////////////////////

/**
 * @brief Program reserved structure.
 */
struct Eng::Program::Reserved
{
   Type type;                                                  ///< Program type   
   std::vector<std::reference_wrapper<Eng::Shader>> shader;    ///< Shaders used by the program
   GLuint oglId;                                               ///< OpenGL program ID   
   std::unordered_map<std::string, GLint> location;            ///< Lookup table for uniform locations


   /**
    * Constructor.
    */
   Reserved() : type{ Eng::Program::Type::none }, oglId{ 0 }
   {}
};



///////////////////////////
// BODY OF CLASS Program //
///////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Constructor.
 */
ENG_API Eng::Program::Program() : reserved(std::make_unique<Eng::Program::Reserved>())
{
   ENG_LOG_DETAIL("[+]");
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Constructor with name.
 * @param name node name
 */
ENG_API Eng::Program::Program(const std::string &name) : Eng::Object(name), reserved(std::make_unique<Eng::Program::Reserved>())
{
   ENG_LOG_DETAIL("[+]");
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Move constructor.
 */
ENG_API Eng::Program::Program(Program &&other) : Eng::Object(std::move(other)), Eng::Managed(std::move(other)), reserved(std::move(other.reserved))
{
   ENG_LOG_DETAIL("[M]");
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Destructor.
 */
ENG_API Eng::Program::~Program()
{
   ENG_LOG_DETAIL("[-]");
   if (reserved)
      this->free();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Create an OpenGL program. 
 * @return TF
 */
bool ENG_API Eng::Program::init()
{	
   if (this->Eng::Managed::init() == false)
      return false;

   // Free program if stored:
   if (reserved->oglId)   
   {  
      glDeleteProgram(reserved->oglId);      
      reserved->oglId = 0;
   }   
	
	// Create program:
	reserved->oglId = glCreateProgram();
	if (reserved->oglId == 0)
	{
      ENG_LOG_ERROR("Unable to create program");      
		return false;
	}

   // Done:      
   return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Destroy an OpenGL instance. 
 * @return TF
 */
bool ENG_API Eng::Program::free() 
{	   
   if (this->Eng::Managed::free() == false)
      return false;   

   // Free shader if stored:
   if (reserved->oglId)   
   {
      glDeleteProgram(reserved->oglId);      
      reserved->oglId = 0;
   }   

   // Done:      
   return true;
}	 


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Get type.
 * @return program type
 */
const Eng::Program::Type ENG_API Eng::Program::getType() const
{
   return reserved->type;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Get number of shaders.
 * @return number of shaders
 */
const uint32_t ENG_API Eng::Program::getNrOfShaders() const
{
   return static_cast<uint32_t>(reserved->shader.size());
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Get the shader at the specified position.
 * @param id shader position in the list
 * @return shader reference
 */
const Eng::Shader ENG_API &Eng::Program::getShader(uint32_t id) const
{
   // Safety net:
   if (id >= reserved->shader.size())
   {
      ENG_LOG_ERROR("Invalid params");
      return Eng::Shader::empty;
   }
   return reserved->shader[id];
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Get the last rendered program.
 * @return last rendered program
 */
Eng::Program ENG_API &Eng::Program::getCached()
{
   return cache;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Build program.
 * @param args variadic list of arguments
 * @return TF
 */
bool ENG_API Eng::Program::build(std::initializer_list<std::reference_wrapper<Eng::Shader>> args)
{
   reserved->shader.clear();
   for (auto &arg : args)
   {
      if (arg.get() == Eng::Shader::empty)
      {
         ENG_LOG_ERROR("Invalid params (empty shader)");
         return false;
      }
      /*if (std::find(reserved->shader.begin(), reserved->shader.end(), arg.get()) != reserved->shader.end())
      {
         OV_LOG_ERROR("Invalid params (same shader passed twice)");
         return false;
      }*/
      reserved->shader.push_back(arg);
   }

   // Validate possible configurations:
   if (this->getNrOfShaders() >= 3)
   {
      ENG_LOG_ERROR("Invalid/unsupported shader sequence");
      return false;
   }

   // Init program:
   this->init();

   // Link shaders:
   for (uint32_t c = 0; c < this->getNrOfShaders(); c++)
   {
      const Eng::Shader &s = dynamic_cast<const Eng::Shader &>(this->getShader(c));
      glAttachShader(reserved->oglId, s.getOglHandle());
   }
   glLinkProgram(reserved->oglId);

   // Check:
   GLint success;
   glGetProgramiv(reserved->oglId, GL_LINK_STATUS, &success);
   if (!success)
   {
      ENG_LOG_ERROR("Program link status error, message:");
      char buffer[Eng::Log::maxLength - 1];
      int32_t length;
      glGetProgramInfoLog(reserved->oglId, Eng::Log::maxLength - 1, &length, buffer);
      if (length > 0)
      {
         ENG_LOG_ERROR("%s", buffer);
      }
      else
      {
         ENG_LOG_ERROR("[no message]");
      }
      return false;
   }

   // Validate:
   glValidateProgram(reserved->oglId);
   glGetProgramiv(reserved->oglId, GL_VALIDATE_STATUS, &success);
   if (success == GL_FALSE)
   {
      ENG_LOG_ERROR("Unable to validate program");
      char buffer[Eng::Log::maxLength - 1];
      int32_t length;
      glGetProgramInfoLog(reserved->oglId, Eng::Log::maxLength - 1, &length, buffer);
      if (length > 0)
      {
         ENG_LOG_ERROR("Program validation error: %s", buffer);
      }
      return false;
   }

   // Done:
   return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Detach program.
 */
void ENG_API Eng::Program::reset()
{
   Eng::Program::cache = Eng::Program::empty;
   glUseProgram(0);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Get parameter location given its name.
 * @param name variable name
 * @return param location or -1 if not found
 */
int32_t ENG_API Eng::Program::getParamLocation(const std::string &name)
{
   // Safety net:
   if (name.empty())
   {
      ENG_LOG_ERROR("Invalid params");
      return -1;
   }

   this->render();

   // Use or add?
   auto location = reserved->location.find(name);
   if (location == reserved->location.end())
   {
      GLint position = glGetUniformLocation(reserved->oglId, name.c_str());
      if (position == -1)
      {
         ENG_LOG_WARN("Variable '%s' not found", name.c_str());
         // return false;
      }
      reserved->location.insert(std::make_pair(name, position));
      location = reserved->location.find(name);
   }

   // Done:      
   return location->second;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Set a uniform value of type float.
 * @param name variable name
 * @param value variable value
 * @return TF
 */
bool ENG_API Eng::Program::setFloat(const std::string &name, float value)
{
   GLint location = getParamLocation(name);
   if (location == -1)
      return false;

   glUniform1f(location, value);
   return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Set a uniform value of type int.
 * @param name variable name
 * @param value variable value
 * @return TF
 */
bool ENG_API Eng::Program::setInt(const std::string &name, int32_t value)
{
   GLint location = getParamLocation(name);
   if (location == -1)
      return false;

   // Done:
   glUniform1i(location, value);
   return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Set a uniform value of type unsigned int.
 * @param name variable name
 * @param value variable value
 * @return TF
 */
bool ENG_API Eng::Program::setUInt(const std::string &name, uint32_t value)
{
   GLint location = getParamLocation(name);
   if (location == -1)
      return false;

   // Done:
   glUniform1ui(location, value);
   return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Set a uniform value of type unsigned int 64.
 * @param name variable name
 * @param value variable value
 * @return TF
 */
bool ENG_API Eng::Program::setUInt64(const std::string &name, uint64_t value)
{
   GLint location = getParamLocation(name);
   if (location == -1)
      return false;

   // Done:
   glUniformHandleui64ARB(location, value);
   return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Set a uniform value of type vec3.
 * @param name variable name
 * @param value variable value
 * @return TF
 */
bool ENG_API Eng::Program::setVec3(const std::string &name, const glm::vec3 &value)
{
   GLint location = getParamLocation(name);
   if (location == -1)
      return false;

   // Done:
   glUniform3fv(location, 1, glm::value_ptr(value));
   return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Set a uniform value of type vec4.
 * @param name variable name
 * @param value variable value
 * @return TF
 */
bool ENG_API Eng::Program::setVec4(const std::string &name, const glm::vec4 &value)
{
   GLint location = getParamLocation(name);
   if (location == -1)
      return false;

   // Done:
   glUniform4fv(location, 1, glm::value_ptr(value));
   return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Set a uniform value of type mat3.
 * @param name variable name
 * @param value variable value
 * @return TF
 */
bool ENG_API Eng::Program::setMat3(const std::string &name, const glm::mat3 &value)
{
   GLint location = getParamLocation(name);
   if (location == -1)
      return false;

   // Done:
   glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));
   return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Set a uniform value of type mat4.
 * @param name variable name
 * @param value variable value
 * @return TF
 */
bool ENG_API Eng::Program::setMat4(const std::string &name, const glm::mat4 &value)
{
   GLint location = getParamLocation(name);
   if (location == -1)
      return false;

   // Done:
   glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
   return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Rendering method.
 * @param value generic value
 * @param data generic pointer to any kind of data
 * @return TF
 */
bool ENG_API Eng::Program::render(uint32_t value, void *data) const
{
   // Render only if necessary:   
   if (Eng::Program::cache.get() != *this)
   {
      glUseProgram(reserved->oglId);
      Eng::Program::cache = const_cast<Eng::Program &>(*this);
   }   

   // Done:
   return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Dispatch compute for a compute shader.
 * @param sizeX workgroup size for X
 * @param sizeY (optional) workgroup size for Y
 * @param sizeZ (optional) workgroup size for Z
 * @return TF
 */
bool ENG_API Eng::Program::compute(uint32_t sizeX, uint32_t sizeY, uint32_t sizeZ) const
{
   // TODO: check a compute shader is really attached

   // Run kernel:
   render();
   glDispatchCompute(sizeX, sizeY, sizeZ);

   // Done:
   return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Wait for the termination of the current compute shader.
 * @return TF
 */
bool ENG_API Eng::Program::wait() const
{
   glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
   glFinish();

   // Done:
   return true;
}