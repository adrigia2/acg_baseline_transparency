/**
 * @file		engine_vbo.cpp
 * @brief	OpenGL Vertex Buffer Object (VBO)
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
   Eng::Vbo Eng::Vbo::empty("[empty]");



/////////////////////////
// RESERVED STRUCTURES //
/////////////////////////

/**
 * @brief VBO reserved structure.
 */
struct Eng::Vbo::Reserved
{  
   GLuint oglId;           ///< OpenGL shader ID
   uint32_t nrOfVertices;  ///< Nr. of vertices


   /**
    * Constructor.
    */
   Reserved() : oglId{ 0 }, nrOfVertices{ 0 }
   {}
};



///////////////////////
// BODY OF CLASS Vbo //
///////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Constructor.
 */
ENG_API Eng::Vbo::Vbo() : reserved(std::make_unique<Eng::Vbo::Reserved>())
{
   ENG_LOG_DETAIL("[+]");
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Constructor with name.
 * @param name node name
 */
ENG_API Eng::Vbo::Vbo(const std::string &name) : Eng::Object(name), reserved(std::make_unique<Eng::Vbo::Reserved>())
{
   ENG_LOG_DETAIL("[+]");
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Move constructor.
 */
ENG_API Eng::Vbo::Vbo(Vbo &&other) : Eng::Object(std::move(other)), Eng::Managed(std::move(other)), reserved(std::move(other.reserved))
{
   ENG_LOG_DETAIL("[M]");
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Destructor.
 */
ENG_API Eng::Vbo::~Vbo()
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
uint32_t ENG_API Eng::Vbo::getOglHandle() const
{
   return reserved->oglId;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Return the nr. of vertices loaded in this VBO.
 * @return nr. of vertices
 */
uint32_t ENG_API Eng::Vbo::getNrOfVertices() const
{
   return reserved->nrOfVertices;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Initializes an OpenGL VBO.
 * @return TF
 */
bool ENG_API Eng::Vbo::init()
{
   if (this->Eng::Managed::init() == false)
      return false;

   // Free buffer if already stored:
   if (reserved->oglId)   
   {   
	   glDeleteBuffers(1, &reserved->oglId);    
      reserved->oglId = 0;   
      reserved->nrOfVertices = 0;
   }   

	// Create it:		    
   glGenBuffers(1, &reserved->oglId);         

   // Done:   
   return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Releases an OpenGL VBO.
 * @return TF
 */
bool ENG_API Eng::Vbo::free()
{
   if (this->Eng::Managed::free() == false)
      return false;

   // Free VBO if stored:
   if (reserved->oglId)
   {
      glDeleteBuffers(1, &reserved->oglId);
      reserved->oglId = 0;
      reserved->nrOfVertices = 0;
   }

   // Done:   
   return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Create buffer by allocating the required storage.
 * @param nfOfVertices number of vertices to store
 * @param data pointer to the data to copy into the buffer 
 * @return TF
 */
bool ENG_API Eng::Vbo::create(uint32_t nrOfVertices, const void *data)
{	
   // Unit size:
   const uint32_t unitSize = sizeof(VertexData);

   // Init buffer:
   if (!this->isInitialized())
      this->init();   
   
   uint64_t size = nrOfVertices * unitSize;   

	// Fill it:		              
   const GLuint oglId = this->getOglHandle();  
   glBindBuffer(GL_ARRAY_BUFFER, oglId);
   glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW); 

   // Setup interleaved-buffer:
   glBindVertexBuffer(0, oglId, 0, static_cast<GLsizei>(unitSize));   
   uint32_t offset = 0;   
   
   // Vertex position data:
   glVertexAttribFormat(static_cast<GLuint>(Attrib::vertex), 3, GL_FLOAT, GL_FALSE, offset);
   glVertexAttribBinding(static_cast<GLuint>(Attrib::vertex), 0);
   glEnableVertexAttribArray(static_cast<GLuint>(Attrib::vertex));
   offset += sizeof(glm::vec3);
   
   // Normal data:   
   glVertexAttribFormat(static_cast<GLuint>(Attrib::normal), 4, GL_INT_2_10_10_10_REV, GL_TRUE, offset);
   glVertexAttribBinding(static_cast<GLuint>(Attrib::normal), 0);
   glEnableVertexAttribArray(static_cast<GLuint>(Attrib::normal));
   offset += sizeof(uint32_t); // 1x compressed vector  

   // Texture coordinates:
   glVertexAttribFormat(static_cast<GLuint>(Attrib::texcoord), 2, GL_HALF_FLOAT, GL_FALSE, offset);
   glVertexAttribBinding(static_cast<GLuint>(Attrib::texcoord), 0);
   glEnableVertexAttribArray(static_cast<GLuint>(Attrib::texcoord));
   offset += sizeof(float); // 2x half float
   
   // Tangent data:
   glVertexAttribFormat(static_cast<GLuint>(Attrib::tangent), 4, GL_INT_2_10_10_10_REV, GL_TRUE, offset);
   glVertexAttribBinding(static_cast<GLuint>(Attrib::tangent), 0);
   glEnableVertexAttribArray(static_cast<GLuint>(Attrib::tangent));
   offset += sizeof(uint32_t); // 1x compressed vector

   // Done:
   reserved->nrOfVertices = nrOfVertices;
   return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Rendering method. 
 * @param value generic value
 * @param data generic pointer to any kind of data
 * @return TF
 */
bool ENG_API Eng::Vbo::render(uint32_t value, void *data) const
{	   
   glBindBuffer(GL_ARRAY_BUFFER, reserved->oglId);
   
   // Done:
   return true;
}
