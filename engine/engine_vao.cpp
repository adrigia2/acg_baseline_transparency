/**
 * @file		engine_vao.cpp
 * @brief	OpenGL Vertex Array Object (VAO)
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
   Eng::Vao Eng::Vao::empty("[empty]");



/////////////////////////
// RESERVED STRUCTURES //
/////////////////////////

/**
 * @brief VAO reserved structure.
 */
struct Eng::Vao::Reserved
{  
   GLuint oglId;        ///< OpenGL shader ID


   /**
    * Constructor.
    */
   Reserved() : oglId{ 0 }
   {}
};



///////////////////////
// BODY OF CLASS Vao //
///////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Constructor.
 */
ENG_API Eng::Vao::Vao() : reserved(std::make_unique<Eng::Vao::Reserved>())
{
   ENG_LOG_DETAIL("[+]");
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Constructor with name.
 * @param name node name
 */
ENG_API Eng::Vao::Vao(const std::string &name) : Eng::Object(name), reserved(std::make_unique<Eng::Vao::Reserved>())
{
   ENG_LOG_DETAIL("[+]");
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Move constructor.
 */
ENG_API Eng::Vao::Vao(Vao &&other) : Eng::Object(std::move(other)), Eng::Managed(std::move(other)), reserved(std::move(other.reserved))
{
   ENG_LOG_DETAIL("[M]");
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Destructor.
 */
ENG_API Eng::Vao::~Vao()
{
   ENG_LOG_DETAIL("[-]");
   if (reserved)
      this->free();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Return the GLuint shader ID.
 * @return shader ID or 0 if not valid
 */
uint32_t ENG_API Eng::Vao::getOglHandle() const
{
   return reserved->oglId;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Initializes an OpenGL VAO.
 * @return TF
 */
bool ENG_API Eng::Vao::init()
{
   if (this->Eng::Managed::init() == false)
      return false;

   // Free buffer if already stored:
   if (reserved->oglId)
   {
      glDeleteVertexArrays(1, &reserved->oglId);
      reserved->oglId = 0;
   }

   // Create it:		       
   glGenVertexArrays(1, &reserved->oglId);

   // Done:   
   return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Releases an OpenGL VAO.
 * @return TF
 */
bool ENG_API Eng::Vao::free()
{
   if (this->Eng::Managed::free() == false)
      return false;

   // Free VAO if stored:
   if (reserved->oglId)
   {
      glDeleteVertexArrays(1, &reserved->oglId);
      reserved->oglId = 0;
   }

   // Done:   
   return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Unbind any vertex array object.  
 */
void ENG_API Eng::Vao::reset()
{	   
	glBindVertexArray(0);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Rendering method. 
 * @param value generic value
 * @param data generic pointer to any kind of data
 * @return TF
 */
bool ENG_API Eng::Vao::render(uint32_t value, void *data) const
{	   
   glBindVertexArray(reserved->oglId);
   
   // Done:
   return true;
}
