/**
 * @file		engine_material.cpp
 * @brief	Physically-Based Rendering (PBR) material properties
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
   Eng::Material Eng::Material::empty("[empty]");



/////////////////////////
// RESERVED STRUCTURES //
/////////////////////////

/**
 * @brief Material reserved structure.
 */
struct Eng::Material::Reserved
{
   // Keep these vars first and in this order...:
   glm::vec3 emission;                                   ///< Emissive term
   float opacity;                                        ///< Transparency (1 = solid, 0 = invisible)
   glm::vec3 albedo;                                     ///< Albedo color
   float roughness;                                      ///< Roughness
   float metalness;                                      ///< Metalness   
   glm::vec3 _pad;                                       ///< Padding   
   // ...48 bytes

   std::reference_wrapper<const Eng::Texture> texture[Eng::Material::maxNrOfTextures];


   /**
    * Constructor.
    */
   Reserved() : emission{ 0.0f },
                albedo{ 0.6f },
                opacity{ 1.0f },
                roughness{ 0.5f }, metalness{ 0.01f }, 
                _pad{ 0.0f },
                texture{ Eng::Texture::empty, Eng::Texture::empty, Eng::Texture::empty, Eng::Texture::empty }
   {}
};



////////////////////////////
// BODY OF CLASS Material //
////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Constructor.
 */
ENG_API Eng::Material::Material() : reserved(std::make_unique<Eng::Material::Reserved>())
{
   ENG_LOG_DETAIL("[+]");
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Constructor with name.
 * @param name node name
 */
ENG_API Eng::Material::Material(const std::string &name) : Eng::Object(name), reserved(std::make_unique<Eng::Material::Reserved>())
{
   ENG_LOG_DETAIL("[+]");
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Move constructor.
 */
ENG_API Eng::Material::Material(Material &&other) : Eng::Object(std::move(other)), reserved(std::move(other.reserved))
{
   ENG_LOG_DETAIL("[M]");
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Destructor.
 */
ENG_API Eng::Material::~Material()
{
   ENG_LOG_DETAIL("[-]");
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Set the emissive component.
 * @param emission emissive term
 */
void ENG_API Eng::Material::setEmission(const glm::vec3 &emission)
{
   reserved->emission = emission;
   setDirty(true);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Set the albedo component.
 * @param albedo term
 */
void ENG_API Eng::Material::setAlbedo(const glm::vec3 &albedo)
{
   reserved->albedo = albedo;
   setDirty(true);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Set the roughness level.
 * @param roughness roughness level
 */
void ENG_API Eng::Material::setRoughness(float roughness)
{
   reserved->roughness = roughness;
   setDirty(true);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Set the metalness level.
 * @param metalness metalness level
 */
void ENG_API Eng::Material::setMetalness(float metalness)
{
   reserved->metalness = metalness;
   setDirty(true);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Get the emissive component.
 * @return emissive term
 */
const glm::vec3 ENG_API &Eng::Material::getEmission() const
{
   return reserved->emission;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Get the albedo component.
 * @return albedo term
 */
const glm::vec3 ENG_API &Eng::Material::getAlbedo() const
{
   return reserved->albedo;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Get the roughness level.
 * @return roughness level
 */
float ENG_API Eng::Material::getRoughness() const
{
   return reserved->roughness;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Get the metalness level.
 * @return metalness level
 */
float ENG_API Eng::Material::getMetalness() const
{
   return reserved->metalness;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Set the opacity of the material (0 = invisibile, 1 = solid).
 * @param opacity opacity level
 */
void ENG_API Eng::Material::setOpacity(float opacity)
{
   reserved->opacity = opacity;
   setDirty(true);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Get the opacity level of the material.
 * @return opacity level
 */
float ENG_API Eng::Material::getOpacity() const
{
   return reserved->opacity;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Sets texture. 
 * @param tex texture
 * @param type texture level
 * @return TF
 */
bool ENG_API Eng::Material::setTexture(const Eng::Texture &tex, Eng::Texture::Type type)
{  
   // Set texture accordingly:
   switch (type)
   {  
      case Eng::Texture::Type::albedo:    reserved->texture[0] = tex; break;
      case Eng::Texture::Type::normal:    reserved->texture[1] = tex; break;
      case Eng::Texture::Type::roughness: reserved->texture[2] = tex; break;
      case Eng::Texture::Type::metalness: reserved->texture[3] = tex; break;
      default:
         ENG_LOG_ERROR("Unsupported texture level");
         return false;
   }

   // Done:
   return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Gets texture. 
 * @param type texture level
 * @return texture at level
 */
const Eng::Texture ENG_API &Eng::Material::getTexture(Eng::Texture::Type type) const
{
   // Get texture:
   switch (type)
   {
      case Eng::Texture::Type::albedo:    return reserved->texture[0];    
      case Eng::Texture::Type::normal:    return reserved->texture[1];
      case Eng::Texture::Type::roughness: return reserved->texture[2];
      case Eng::Texture::Type::metalness: return reserved->texture[3];
      default:
         ENG_LOG_ERROR("Unsupported texture level");
         return Eng::Texture::empty;
   }
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Loads the specific information of a given object. In its base class, this function loads the file version chunk.
 * @param serializer serial data
 * @param data optional pointer
 * @return 1 on success, 0 if error
 */
uint32_t ENG_API Eng::Material::loadChunk(Eng::Serializer &serial, void *data)
{
   // Chunk header:
   uint32_t chunkId;
   serial.deserialize(&chunkId, sizeof(uint32_t));
   if (chunkId != static_cast<uint32_t>(Ovo::ChunkId::material))
   {
      ENG_LOG_ERROR("Invalid chunk ID found");
      return 0;
   }
   uint32_t chunkSize;
   serial.deserialize(&chunkSize, sizeof(uint32_t));

   // Material properties:
   std::string name;
   serial.deserialize(name);
   this->setName(name);

   // PBR props:   
   serial.deserialize(reserved->emission);
   serial.deserialize(reserved->albedo);
   serial.deserialize(reserved->roughness);
   serial.deserialize(reserved->metalness);
   serial.deserialize(reserved->opacity);
   

   // Textures:
   Eng::Container &container = Eng::Container::getInstance();

   // Albedo:
   serial.deserialize(name); 
   ENG_LOG_PLAIN("Texture (albedo): %s", name.c_str());
   if (name != "[none]")
   {
      Eng::Bitmap bitmap;
      if (!bitmap.load(name))
         ENG_LOG_ERROR("Unable to load image file '%s'", name.c_str());
      else
      {
         Eng::Texture tex;
         tex.load(bitmap);
         container.add(tex);     
         this->setTexture(container.getLastTexture(), Eng::Texture::Type::albedo);
      }
   }

   // Normal:
   serial.deserialize(name); 
   ENG_LOG_PLAIN("Texture (normal): %s", name.c_str());
   if (name != "[none]")
   {
      Eng::Bitmap bitmap;
      if (!bitmap.load(name))
         ENG_LOG_ERROR("Unable to load image file '%s'", name.c_str());
      else
      {
         container.add(bitmap);
         Eng::Texture tex;
         tex.load(container.getLastBitmap());
         container.add(tex);     
         this->setTexture(container.getLastTexture(), Eng::Texture::Type::normal);
      }
   }

   // Height (ignored):
   serial.deserialize(name); 
   ENG_LOG_PLAIN("Texture (height): %s", name.c_str());
   
   // Roughness:
   serial.deserialize(name); 
   ENG_LOG_PLAIN("Texture (roughness): %s", name.c_str());
   if (name != "[none]")
   {
      Eng::Bitmap bitmap;
      if (!bitmap.load(name))
         ENG_LOG_ERROR("Unable to load image file '%s'", name.c_str());
      else
      {
         container.add(bitmap);
         Eng::Texture tex;
         tex.load(container.getLastBitmap());
         container.add(tex);     
         this->setTexture(container.getLastTexture(), Eng::Texture::Type::roughness);
      }
   }

   // Metalness:
   serial.deserialize(name); 
   ENG_LOG_PLAIN("Texture (metalness): %s", name.c_str());
   if (name != "[none]")
   {
      Eng::Bitmap bitmap;
      if (!bitmap.load(name))
         ENG_LOG_ERROR("Unable to load image file '%s'", name.c_str());
      else
      {
         container.add(bitmap);
         Eng::Texture tex;
         tex.load(container.getLastBitmap());
         container.add(tex);     
         this->setTexture(container.getLastTexture(), Eng::Texture::Type::metalness);
      }
   }   

   // Done:
   return 1;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Rendering method.
 * @param value generic value
 * @param data generic pointer to any kind of data
 * @return TF
 */
bool ENG_API Eng::Material::render(uint32_t value, void *data) const
{	
   Eng::Program &prog = Eng::Program::getCached();

   // Pass (some) params:
   prog.setVec3("mtlEmission", reserved->emission);
   prog.setVec3("mtlAlbedo", reserved->albedo);
   prog.setFloat("mtlRoughness", reserved->roughness);
   prog.setFloat("mtlOpacity", reserved->opacity);
    
   // Pass textures:
   for (uint32_t c = 0; c < Eng::Material::maxNrOfTextures; c++)
      if (reserved->texture[c].get() != Eng::Texture::empty)
         reserved->texture[c].get().render(c);
      else
         Eng::Texture::getDefault().render(c);

   // Done:
   return true;
}
