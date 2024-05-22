/**
 * @file		engine_mesh.h
 * @brief	Geometric mesh 
 *
 * @author	Achille Peternier (achille.peternier@supsi.ch), (C) SUPSI
 */
#pragma once



/**
 * @brief Class for modeling a geometric mesh.
 */
class ENG_API Mesh final : public Eng::Node
{
//////////
public: //
//////////

   // Special values:
   static Mesh empty;   

   // Const/dest:
   Mesh();
   Mesh(Mesh &&other);
   Mesh(Mesh const &) = delete;
   ~Mesh();   

   // Operators:
   void operator=(Mesh const&) = delete;

   // Get/set:
   bool setMaterial(const Eng::Material &mat);
   const Eng::Material &getMaterial() const;
   
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
   Mesh(const std::string &name);
};





