/**
 * @file		engine_vao.h
 * @brief	OpenGL Vertex Array Object (VAO)
 *
 * @author	Achille Peternier (achille.peternier@supsi.ch), (C) SUPSI
 */
#pragma once



/**
 * @brief Class for modeling a Vertex Array Object (VAO).
 */
class ENG_API Vao final : public Eng::Object, public Eng::Managed
{
//////////
public: //
//////////

   // Special values:
   static Vao empty;

   // Const/dest:
   Vao();
   Vao(Vao &&other);
   Vao(Vao const &) = delete;
   ~Vao();   
   
   // Get/set:   
   uint32_t getOglHandle() const;

   // Rendering methods:
   static void reset();
   bool render(uint32_t value = 0, void *data = nullptr) const;

   // Managed:
   bool init() override;
   bool free() override;


///////////
private: //
///////////

   // Reserved:
   struct Reserved;
   std::unique_ptr<Reserved> reserved;

   // Const/dest:
   Vao(const std::string &name);   
};



