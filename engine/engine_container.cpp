/**
 * @file		engine_container.cpp
 * @brief	Centralized data container
 *
 * @author	Achille Peternier (achille.peternier@supsi.ch), (C) SUPSI
 */



//////////////
// #INCLUDE //
//////////////

    // Main include:
   #include "engine.h"

   // C/C++:
   #include <algorithm>
   #include <variant>



////////////
// STATIC //
////////////

   // Special values:
   Eng::Container Eng::Container::empty("[empty]");



/////////////////////////
// RESERVED STRUCTURES //
/////////////////////////

/**
 * @brief Container reserved structure.
 */
struct Eng::Container::Reserved
{
   std::list<Eng::Node> allNodes;
   std::list<Eng::Mesh> allMeshes;
   std::list<Eng::Camera> allCameras;
   std::list<Eng::Light> allLights;
   std::list<Eng::Bitmap> allBitmaps;
   std::list<Eng::Material> allMaterials;
   std::list<Eng::Texture> allTextures;
   

   /**
    * Constructor.
    */
   Reserved()
   {}
};



/////////////////////////////
// BODY OF CLASS Container //
/////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Constructor.
 */
ENG_API Eng::Container::Container() : reserved(std::make_unique<Eng::Container::Reserved>())
{
   ENG_LOG_DETAIL("[+]");
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Constructor with name.
 * @param name node name
 */
ENG_API Eng::Container::Container(const std::string &name) : Eng::Object(name), reserved(std::make_unique<Eng::Container::Reserved>())
{
   ENG_LOG_DETAIL("[+]");
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Move constructor.
 */
ENG_API Eng::Container::Container(Container &&other) : Eng::Object(std::move(other)), reserved(std::move(other.reserved))
{
   ENG_LOG_DETAIL("[M]");
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Destructor.
 */
ENG_API Eng::Container::~Container()
{
   ENG_LOG_DETAIL("[-]");
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Get singleton instance.
 */
Eng::Container ENG_API &Eng::Container::getInstance()
{
   static Container instance("[default]");
   return instance;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Gets last added node. 
 * @return last added node
 */
Eng::Node ENG_API &Eng::Container::getLastNode() const
{
   // Safety net:
   if (reserved->allNodes.empty())
      return Eng::Node::empty;
   return reserved->allNodes.back();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Gets last added mesh.
 * @return last added mesh
 */
Eng::Mesh ENG_API &Eng::Container::getLastMesh() const
{
   // Safety net:
   if (reserved->allMeshes.empty())
      return Eng::Mesh::empty;
   return reserved->allMeshes.back();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Gets last added camera.
 * @return last added camera
 */
Eng::Camera ENG_API &Eng::Container::getLastCamera() const
{
   // Safety net:
   if (reserved->allCameras.empty())
      return Eng::Camera::empty;
   return reserved->allCameras.back();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Gets last added light.
 * @return last added light
 */
Eng::Light ENG_API &Eng::Container::getLastLight() const
{
   // Safety net:
   if (reserved->allLights.empty())
      return Eng::Light::empty;
   return reserved->allLights.back();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Gets last added bitmap.
 * @return last added bitmap
 */
Eng::Bitmap ENG_API &Eng::Container::getLastBitmap() const
{
   // Safety net:
   if (reserved->allBitmaps.empty())
      return Eng::Bitmap::empty;
   return reserved->allBitmaps.back();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Gets last added material.
 * @return last added material
 */
Eng::Material ENG_API &Eng::Container::getLastMaterial() const
{
   // Safety net:
   if (reserved->allMaterials.empty())
      return Eng::Material::empty;
   return reserved->allMaterials.back();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Gets last added texture.
 * @return last added texture
 */
Eng::Texture ENG_API &Eng::Container::getLastTexture() const
{
   // Safety net:
   if (reserved->allTextures.empty())
      return Eng::Texture::empty;
   return reserved->allTextures.back();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Gets direct access to the list of nodes.
 * @return list of nodes
 */
std::list<Eng::Node> ENG_API &Eng::Container::getNodeList()
{  
   return reserved->allNodes;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Gets direct access to the list of meshes.
 * @return list of meshes
 */
std::list<Eng::Mesh> ENG_API &Eng::Container::getMeshList()
{
   return reserved->allMeshes;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Gets direct access to the list of cameras.
 * @return list of cameras
 */
std::list<Eng::Camera> ENG_API &Eng::Container::getCameraList()
{
   return reserved->allCameras;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Gets direct access to the list of lights.
 * @return list of lights
 */
std::list<Eng::Light> ENG_API &Eng::Container::getLightList()
{
   return reserved->allLights;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Gets direct access to the list of bitmaps.
 * @return list of bitmaps
 */
std::list<Eng::Bitmap> ENG_API &Eng::Container::getBitmapList()
{
   return reserved->allBitmaps;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Gets direct access to the list of materials.
 * @return list of materials
 */
std::list<Eng::Material> ENG_API &Eng::Container::getMaterialList()
{
   return reserved->allMaterials;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Gets direct access to the list of textures.
 * @return list of textures
 */
std::list<Eng::Texture> ENG_API &Eng::Container::getTextureList()
{
   return reserved->allTextures;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Returns, if existing, the first object with the given name among its various lists. 
 * @param name object name
 * @return found object or empty
 */
Eng::Object ENG_API &Eng::Container::find(const std::string &name) const
{
   // Safety net:
   if (name.empty())
   {
      ENG_LOG_ERROR("Invalid params");
      return Eng::Object::empty;
   }

   // Seach in materials:
   for (auto &c : reserved->allMaterials)
      if (c.getName() == name)
         return c;

   // Seach in textures:
   for (auto &c : reserved->allTextures)
      if (c.getName() == name)
         return c;
   
   // Seach in geometries:
   for (auto &c : reserved->allMeshes)
      if (c.getName() == name)
         return c;

   // Seach in cameras:
   for (auto &c : reserved->allCameras)
      if (c.getName() == name)
         return c;

   // Seach in lights:
   for (auto &c : reserved->allLights)
      if (c.getName() == name)
         return c;

   // Seach in nodes:
   for (auto &c : reserved->allNodes)
      if (c.getName() == name)
         return c;

   // Seach in bitmaps:
   for (auto &c : reserved->allBitmaps)
      if (c.getName() == name)
         return c;
   
   // Not found:
   return Eng::Object::empty;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Returns, if existing, the first object with the given ID among its various lists.
 * @param id object id
 * @return found object or empty
 */
Eng::Object ENG_API &Eng::Container::find(const uint32_t id) const
{
   // Fast lane:
   if (id == 0)         
      return Eng::Object::empty;   
   
   // Seach in materials:
   for (auto &c : reserved->allMaterials)
      if (c.getId() == id)
         return c;

   // Seach in textures:
   for (auto &c : reserved->allTextures)
      if (c.getId() == id)
         return c;

   // Seach in geometries:
   for (auto &c : reserved->allMeshes)
      if (c.getId() == id)
         return c;

   // Seach in cameras:
   for (auto &c : reserved->allCameras)
      if (c.getId() == id)
         return c;

   // Seach in lights:
   for (auto &c : reserved->allLights)
      if (c.getId() == id)
         return c;

   // Seach in nodes:
   for (auto &c : reserved->allNodes)
      if (c.getId() == id)
         return c;

   // Seach in bitmaps:
   for (auto &c : reserved->allBitmaps)
      if (c.getId() == id)
         return c;

   // Not found:
   return Eng::Object::empty;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Resets the content of the container. 
 * @return TF
 */
bool ENG_API Eng::Container::reset()
{
   reserved->allNodes.clear();
   reserved->allMeshes.clear();   
   reserved->allCameras.clear();
   reserved->allLights.clear();
   reserved->allBitmaps.clear();
   reserved->allMaterials.clear();   
   reserved->allTextures.clear();   
   
   // Done:
   setDirty(true);
   return true; 
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Adds the given object to the proper container.
 * @return TF
 */
bool ENG_API Eng::Container::add(Eng::Object &obj)
{
   // Safety net:
   if (obj == Eng::Object::empty)
   {
      ENG_LOG_ERROR("Invalid params");
      return false;
   }

   // Sort by type:
   if (dynamic_cast<Eng::Mesh *>(&obj))
   {
      reserved->allMeshes.push_back(std::move(dynamic_cast<Eng::Mesh &>(obj)));      
      return true;
   }
   else
      if (dynamic_cast<Eng::Camera *>(&obj))
      {
         reserved->allCameras.push_back(std::move(dynamic_cast<Eng::Camera &>(obj)));
         return true;
      }
      else
         if (dynamic_cast<Eng::Light *>(&obj))
         {
            reserved->allLights.push_back(std::move(dynamic_cast<Eng::Light &>(obj)));      
            return true;
         }
         else      
            if (dynamic_cast<Eng::Node *>(&obj))
            {
               reserved->allNodes.push_back(std::move(dynamic_cast<Eng::Node &>(obj)));         
               return true;
            }      
            else
               if (dynamic_cast<Eng::Material *>(&obj))
               {
                  reserved->allMaterials.push_back(std::move(dynamic_cast<Eng::Material &>(obj)));         
                  return true;
               }      
               else
                  if (dynamic_cast<Eng::Texture *>(&obj))
                  {
                     reserved->allTextures.push_back(std::move(dynamic_cast<Eng::Texture &>(obj)));         
                     return true;
                  }      
                  else
                     if (dynamic_cast<Eng::Bitmap *>(&obj))
                     {
                        reserved->allBitmaps.push_back(std::move(dynamic_cast<Eng::Bitmap &>(obj)));
                        return true;
                     }
   
   // Done:
   ENG_LOG_ERROR("Unsupported type");   
   return false;
}