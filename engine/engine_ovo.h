/**
 * @file		engine_ovo.h
 * @brief	OVO importing 
 *
 * @author	Achille Peternier (achille.peternier@supsi.ch), (C) SUPSI
 */
#pragma once



///////////////
// PREDEFINE //
///////////////

   // Several predefines are required, as many later classes will derive from this one:
   class Node; 



/**
 * @brief Base 3D OVO manager.
 */
class ENG_API Ovo 
{
//////////
public: //
//////////	   

   // Consts:
   static constexpr uint32_t version = 8;       ///< OVO format revision (divide by 10)   


   /**
    * @brief Chunk IDs (taken from the OVO format specs).
    */
   enum class ChunkId : uint32_t
   {
      version  = 0,      
      node     = 1,
      material = 9,      
      light    = 16,
      mesh     = 18,      

      // Terminator:
      last
   };


   // Loading methods:
   Eng::Node &load(const std::string &filename);
   virtual uint32_t loadChunk(Eng::Serializer &serial, void *data = nullptr);
   uint32_t ignoreChunk(Eng::Serializer &serial);
};

