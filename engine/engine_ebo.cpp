/**
 * @file		engine_ebo.cpp
 * @brief	OpenGL Element Array Buffer Object (EBO)
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
   Eng::Ebo Eng::Ebo::empty("[empty]");



/////////////////////////
// RESERVED STRUCTURES //
/////////////////////////

/**
 * @brief EBO reserved structure.
 */
struct Eng::Ebo::Reserved
{  
   GLuint oglId;        ///< OpenGL shader ID
   uint32_t nrOfFaces;  ///< Nr. of faces


   /**
    * Constructor.
    */
   Reserved() : oglId{ 0 }, nrOfFaces{ 0 }
   {}
};



///////////////////////
// BODY OF CLASS Ebo //
///////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Constructor.
 */
ENG_API Eng::Ebo::Ebo() : reserved(std::make_unique<Eng::Ebo::Reserved>())
{
   ENG_LOG_DETAIL("[+]");
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Constructor with name.
 * @param name node name
 */
ENG_API Eng::Ebo::Ebo(const std::string &name) : Eng::Object(name), reserved(std::make_unique<Eng::Ebo::Reserved>())
{
   ENG_LOG_DETAIL("[+]");
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Move constructor.
 */
ENG_API Eng::Ebo::Ebo(Ebo &&other) : Eng::Object(std::move(other)), Eng::Managed(std::move(other)), reserved(std::move(other.reserved))
{
   ENG_LOG_DETAIL("[M]");
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Destructor.
 */
ENG_API Eng::Ebo::~Ebo()
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
uint32_t ENG_API Eng::Ebo::getOglHandle() const
{
   return reserved->oglId;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Return the nr. of faces loaded in this EBO.
 * @return nr. of faces
 */
uint32_t ENG_API Eng::Ebo::getNrOfFaces() const
{
   return reserved->nrOfFaces;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Initializes an OpenGL EBO.
 * @return TF
 */
bool ENG_API Eng::Ebo::init()
{
   if (this->Eng::Managed::init() == false)
      return false;

   // Free buffer if already stored:
   if (reserved->oglId)   
   {   
	   glDeleteBuffers(1, &reserved->oglId);    
      reserved->oglId = 0;   
      reserved->nrOfFaces = 0;
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
bool ENG_API Eng::Ebo::free()
{
   if (this->Eng::Managed::free() == false)
      return false;

   // Free EBO if stored:
   if (reserved->oglId)
   {
      glDeleteBuffers(1, &reserved->oglId);
      reserved->oglId = 0;
      reserved->nrOfFaces = 0;
   }

   // Done:   
   return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Create element buffer by allocating the required storage.
 * @param nfOfFaces number of faces to store
 * @param data pointer to the data to copy into the buffer
 * @return TF
 */
bool ENG_API Eng::Ebo::create(uint32_t nrOfFaces, const void *data)
{	
   // Init buffer:
   if (!this->isInitialized())
      this->init();
   uint64_t size = nrOfFaces * sizeof(FaceData); 

	// Create it:		              
   const GLuint oglId = this->getOglHandle();
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, oglId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW); 

   // Done:
   reserved->nrOfFaces = nrOfFaces;
   return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Rendering method. 
 * @param value generic value
 * @param data generic pointer to any kind of data
 * @return TF
 */
bool ENG_API Eng::Ebo::render(uint32_t value, void *data) const
{	   
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, reserved->oglId);  
   
   // Done:
   return true;
}
