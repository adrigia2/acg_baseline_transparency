/**
 * @file		engine_light.h
 * @brief	Generic light source
 *
 * @author	Achille Peternier (achille.peternier@supsi.ch), (C) SUPSI
 */
#pragma once



/**
 * @brief Class for modeling a generic light. 
 */
class ENG_API Light final : public Eng::Node
{
//////////
public: //
//////////

   // Special values:
   static Light empty;   

   // Const/dest:
	Light();      
	Light(Light &&other);
   Light(Light const&) = delete;   
   virtual ~Light();         

   // Operators:
   void operator=(Light const&) = delete;

   // Get/set:
   void setColor(const glm::vec3 &color);
   const glm::vec3 &getColor() const;
   void setAmbient(const glm::vec3 &ambient);
   const glm::vec3 &getAmbient() const;    
   void setProjMatrix(const glm::mat4 &projMatrix);
   const glm::mat4 &getProjMatrix() const;

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
   Light(const std::string &name);
};
