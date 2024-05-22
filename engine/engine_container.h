/**
 * @file		engine_container.h
 * @brief	Centralized data container
 *
 * @author	Achille Peternier (achille.peternier@supsi.ch), (C) SUPSI
 */
#pragma once



/**
 * @brief Class for storing data used during the life-cycle of the engine. 
 */
class ENG_API Container final : public Eng::Object
{
//////////
public: //
//////////

   // Special values:
   static Container empty;

   // Const/dest:   
   Container(Container const &) = delete;
   virtual ~Container();

   // Singleton:
   static Container &getInstance();

   // Manager:
   bool add(Eng::Object &obj);
   bool reset();

   // Get/set:
   Eng::Node &getLastNode() const;
   Eng::Mesh &getLastMesh() const;   
   Eng::Camera &getLastCamera() const;
   Eng::Light &getLastLight() const;   
   Eng::Bitmap &getLastBitmap() const;
   Eng::Material &getLastMaterial() const;   
   Eng::Texture &getLastTexture() const;   
   std::list<Eng::Node> &getNodeList();
   std::list<Eng::Mesh> &getMeshList();
   std::list<Eng::Camera> &getCameraList();
   std::list<Eng::Light> &getLightList();
   std::list<Eng::Bitmap> &getBitmapList();
   std::list<Eng::Material> &getMaterialList();
   std::list<Eng::Texture> &getTextureList();
   
   // Finders:
   Eng::Object &find(const std::string &name) const;   ///< By name
   Eng::Object &find(uint32_t id) const;               ///< By ID


///////////
private: //
///////////

   // Reserved:
   struct Reserved;
   std::unique_ptr<Reserved> reserved;

   // Const/dest:
   Container(const std::string &name);
   Container();
   Container(Container &&other);

   // Workaround for disabling the unneeded rendering method:
   using Object::render;
};




