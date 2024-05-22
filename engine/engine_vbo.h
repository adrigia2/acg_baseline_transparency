/**
 * @file		engine_vbo.h
 * @brief	OpenGL Vertex Buffer Object (VBO)
 *
 * @author	Achille Peternier (achille.peternier@supsi.ch), (C) SUPSI
 */
#pragma once



/**
 * @brief Class for modeling a Vertex Buffer Object (VBO).
 */
class ENG_API Vbo final : public Eng::Object, public Eng::Managed
{
//////////
public: //
//////////

   // Special values:
   static Vbo empty;


   /**
    * @brief Definition of vertex attrib layers.
    */
   enum class Attrib : uint32_t
   {
      vertex,
      normal,
      texcoord,      
      tangent,      

      // Terminator:
      last
   };

   
   /**
    * @brief Per-vertex data
    */
   struct VertexData	 
	{		
		glm::vec3 vertex;		///< Vertex data, native
      uint32_t normal;		///< Normal, packed as 10_10_10_2
      uint32_t uv;         ///< Tex coords, packed as 2xfp16
      uint32_t tangent;	   ///< Tangent, packed as 10_10_10_2


      /**
       * Constructor. 
       */
      inline VertexData() noexcept : vertex{ 0.0f }, normal{ 0 }, uv{ 0 }, tangent{ 0 }
      {}
	};


   // Const/dest:
   Vbo();
   Vbo(Vbo &&other);
   Vbo(Vbo const &) = delete;
   ~Vbo();   
   
   // Get/set:   
   uint32_t getNrOfVertices() const;
   uint32_t getOglHandle() const;

   // Data:
   bool create(uint32_t nrOfVertices, const void *data = nullptr);

   // Rendering methods:   
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
   Vbo(const std::string &name);
};




