/**
 * @file		engine_ebo.h
 * @brief	OpenGL Element Array Buffer Object (EBO)
 *
 * @author	Achille Peternier (achille.peternier@supsi.ch), (C) SUPSI
 */
#pragma once



/**
 * @brief Class for modeling an Element Array Buffer Object (EBO).
 */
class ENG_API Ebo final : public Eng::Object, public Eng::Managed
{
//////////
public: //
//////////

   // Special values:
   static Ebo empty;


   /** 
    * @brief Per-face data
    */	
   struct FaceData
   {		
		uint32_t a, b, c;


      /**
       * Constructor. 
       */
      inline FaceData() noexcept : a{ 0 }, b{ 0 }, c{ 0 }
      {}
	};      


   // Const/dest:
   Ebo();
   Ebo(Ebo &&other);
   Ebo(Ebo const &) = delete;
   ~Ebo();   
   
   // Get/set:   
   uint32_t getNrOfFaces() const;
   uint32_t getOglHandle() const;

   // Data:
   bool create(uint32_t nrOfFaces, const void *data = nullptr);

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
   Ebo(const std::string &name);
};





