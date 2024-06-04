#include "engine.h"

//////////////
// #INCLUDE //
//////////////

   // Main include:
   #include "engine.h"
    #include "engine_pbo.h"

   // OGL:      
   #include <GL/glew.h>
   #include <GLFW/glfw3.h>



////////////
// STATIC //
////////////

   // Special values:
   Eng::Pbo Eng::Pbo::empty("[empty]");



/////////////////////////
// RESERVED STRUCTURES //
/////////////////////////

/**
 * @brief EBO reserved structure.
 */
struct Eng::Pbo::Reserved
{  
   GLuint oglId;        ///< OpenGL shader ID


   /**
    * Constructor.
    */
   Reserved() : oglId{ 0 }
   {}
};



///////////////////////
// BODY OF CLASS Ebo //
///////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Constructor.
 */
ENG_API Eng::Pbo::Pbo() : reserved(std::make_unique<Eng::Pbo::Reserved>())
{
   ENG_LOG_DETAIL("[+]");
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Constructor with name.
 * @param name node name
 */
ENG_API Eng::Pbo::Pbo(const std::string &name) : Eng::Object(name), reserved(std::make_unique<Eng::Pbo::Reserved>())
{
   ENG_LOG_DETAIL("[+]");
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Move constructor.
 */
ENG_API Eng::Pbo::Pbo(Pbo &&other) : Eng::Object(std::move(other)), Eng::Managed(std::move(other)), reserved(std::move(other.reserved))
{
   ENG_LOG_DETAIL("[M]");
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Destructor.
 */
ENG_API Eng::Pbo::~Pbo()
{
   ENG_LOG_DETAIL("[-]");
   if (reserved)
      this->free();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Return the GLuint object ID.
 * @return object ID or 0 if not valid
 */
uint32_t ENG_API Eng::Pbo::getOglHandle() const
{
   return reserved->oglId;
}




/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Return the nr. of faces loaded in this EBO.
 * @return nr. of faces
 */



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Initializes an OpenGL EBO.
 * @return TF
 */
bool ENG_API Eng::Pbo::init()
{
   if (this->Eng::Managed::init() == false)
      return false;

   // Free buffer if already stored:
   if (reserved->oglId)   
   {   
	   glDeleteBuffers(1, &reserved->oglId);    
      reserved->oglId = 0;   
   }   

	// Create it:		    
   glGenBuffers(1, &reserved->oglId);          

   // Done:   
   return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Releases an OpenGL EBO.
 * @return TF
 */
bool ENG_API Eng::Pbo::free()
{
   if (this->Eng::Managed::free() == false)
      return false;

   // Free EBO if stored:
   if (reserved->oglId)
   {
      glDeleteBuffers(1, &reserved->oglId);
      reserved->oglId = 0;
   }

   // Done:   
   return true;
}

bool Eng::Pbo::create(std::vector<uint32_t>& data)
{
    // Init buffer:
    if (!this->isInitialized())
        this->init();

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, reserved->oglId);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, data.size() * sizeof(GLuint),
        &data[0], GL_STATIC_COPY);

    return true;
}

void Eng::Pbo::reset()
{
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, reserved->oglId);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Rendering method. 
 * @param value generic value
 * @param data generic pointer to any kind of data
 * @return TF
 */
bool ENG_API Eng::Pbo::render(uint32_t value, void *data) const
{	   
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, reserved->oglId);
 
   // Done:
   return true;
}
