/**
 * @file		engine_material.h
 * @brief	Physically-Based Rendering (PBR) material properties
 *
 * @author	Achille Peternier (achille.peternier@supsi.ch), (C) SUPSI
 */
#pragma once



/**
 * @brief Class for modeling a generic PBR material.
 */
class ENG_API Material final : public Eng::Object, public Eng::Ovo
{
//////////
public: //
//////////

   // Special values:
   static Material empty;
   constexpr static uint32_t maxNrOfTextures = 4;     ///< Max number of textures per material
   

   // Const/dest:
   Material();
   Material(Material &&other);   
   Material(Material const &) = delete;
   virtual ~Material();

   // Get/set:
   void setEmission(const glm::vec3 &emission);   
   void setAlbedo(const glm::vec3 &albedo);   
   void setOpacity(float opacity);
   void setRoughness(float roughness);
   void setMetalness(float metalness);   
   const glm::vec3 &getEmission() const;
   const glm::vec3 &getAlbedo() const;   
   float getOpacity() const;
   float getRoughness() const;
   float getMetalness() const;   
   bool setTexture(const Eng::Texture &tex, Eng::Texture::Type type = Eng::Texture::Type::albedo);
   const Eng::Texture &getTexture(Eng::Texture::Type type = Eng::Texture::Type::albedo) const;

   // Rendering methods:   
   bool render(uint32_t value = 0, void *data = nullptr) const;

   // Ovo:   
   uint32_t loadChunk(Eng::Serializer &serial, void *data = nullptr) override;


///////////
private: //
///////////

   // Reserved:
   struct Reserved;
   std::unique_ptr<Reserved> reserved;

   // Const/dest:
   Material(const std::string &name);
};


