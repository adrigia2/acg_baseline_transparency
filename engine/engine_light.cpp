/**
 * @file		engine_light.cpp
 * @brief	Generic light source
 *
 * @author	Achille Peternier (achille.peternier@supsi.ch), (C) SUPSI
 */



//////////////
// #INCLUDE //
//////////////

   // Main include:
   #include "engine.h"

   

////////////
// STATIC //
////////////

   // Special values:
   Eng::Light Eng::Light::empty("[empty]");   



/////////////////////////
// RESERVED STRUCTURES //
/////////////////////////

/**
 * @brief Light reserved structure.
 */
struct Eng::Light::Reserved
{  
   glm::vec3 color;              ///< Light color
   glm::vec3 ambient;            ///< Ambient color
   glm::mat4 projMatrix;         ///< Projection matrix used for shadow mapping


   /**
    * Constructor. 
    */
   Reserved() : color{ 1.0f }, ambient { 0.25f },
                projMatrix{ 1.0f }
   {}
};



/////////////////////////
// BODY OF CLASS Light //
/////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Constructor.
 */
ENG_API Eng::Light::Light() : reserved(std::make_unique<Eng::Light::Reserved>())
{	
   ENG_LOG_DETAIL("[+]");
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Constructor with name.
 * @param name mesh name
 */
ENG_API Eng::Light::Light(const std::string &name) : Eng::Node(name), reserved(std::make_unique<Eng::Light::Reserved>())
{	   	
   ENG_LOG_DETAIL("[+]");
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Move constructor. 
 */
ENG_API Eng::Light::Light(Light &&other) : Eng::Node(std::move(other)), reserved(std::move(other.reserved))
{  
   ENG_LOG_DETAIL("[M]");
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Destructor.
 */
ENG_API Eng::Light::~Light()
{
   ENG_LOG_DETAIL("[-]");
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Set the color of the light.
 * @param color light color
 */
void ENG_API Eng::Light::setColor(const glm::vec3 &color)
{
   reserved->color = color;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Get the light color.
 * @return light color
 */
const glm::vec3 ENG_API &Eng::Light::getColor() const
{
   return reserved->color;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Set the light ambient color.
 * @param ambient light ambient color
 */
void ENG_API Eng::Light::setAmbient(const glm::vec3 &ambient)
{
   reserved->ambient = ambient;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Get the light ambient color.
 * @return light ambient color
 */
const glm::vec3 ENG_API &Eng::Light::getAmbient() const
{
   return reserved->ambient;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Sets light projection matrix used for shadow mapping.
 * @param projMatrix projection matrix as glm mat4x4 
 */
void ENG_API Eng::Light::setProjMatrix(const glm::mat4 &projMatrix) 
{		
   reserved->projMatrix = projMatrix;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Gets light projection matrix used for shadow mapping. 
 * @return projection matrix as glm 4x4 matrix
 */
const glm::mat4 ENG_API &Eng::Light::getProjMatrix() const
{	
   return reserved->projMatrix;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Loads the specific information of a given object. In its base class, this function loads the file version chunk.
 * @param serializer serial data
 * @param data optional pointer
 * @return TF
 */
uint32_t ENG_API Eng::Light::loadChunk(Eng::Serializer &serial, void *data)
{
   // Chunk header
   uint32_t chunkId;
   serial.deserialize(&chunkId, sizeof(uint32_t));
   if (chunkId != static_cast<uint32_t>(Ovo::ChunkId::light))
   {
      ENG_LOG_ERROR("Invalid chunk ID found");
      return 0;
   }
   uint32_t chunkSize;
   serial.deserialize(&chunkSize, sizeof(uint32_t));

   // Node properties:       
   std::string name;
   serial.deserialize(name);   
   this->setName(name);

   glm::mat4 matrix;
   serial.deserialize(matrix);
   this->setMatrix(matrix);

   uint32_t nrOfChildren;
   serial.deserialize(nrOfChildren);

   std::string target;
   serial.deserialize(target);   
   
   // Data:
   uint8_t subtype;
   serial.deserialize(subtype);

   serial.deserialize(reserved->color);
   float radius;
   serial.deserialize(radius);
   glm::vec3 direction;
   serial.deserialize(direction);
   float cutoff;
   serial.deserialize(cutoff);
   float spotExponent;
   serial.deserialize(spotExponent);
   uint8_t castShadows;
   serial.deserialize(castShadows);
   uint8_t isVolumetric;
   serial.deserialize(isVolumetric);  

   // Done:      
   return nrOfChildren;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Rendering method. 
 * @param value generic value
 * @param data generic pointer to any kind of data
 * @return TF
 */
bool ENG_API Eng::Light::render(uint32_t value, void *data) const
{	   
   Eng::Program &program = dynamic_cast<Eng::Program &>(Eng::Program::getCached());
   program.setVec3("lightColor", reserved->color);   
   program.setVec3("lightAmbient", reserved->ambient);   
   program.setVec3("lightPosition", glm::vec3((*((glm::mat4 *) data))[3]));
   
   // Done:
   return true;
}
