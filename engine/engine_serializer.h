/**
 * @file		engine_serializer.h
 * @brief	Basic generic serializer
 *
 * @author	Achille Peternier (achille.peternier@supsi.ch), (C) SUPSI
 */
#pragma once



/**
 * @brief Class for (de)serializing data (from)to memory.
 */
class ENG_API Serializer final
{
//////////
public: //
//////////

   // Special values:
   static Serializer empty;      

   // Const/dest:
   Serializer();   
   Serializer(const Serializer &other);   
   Serializer(const void *rawData, uint64_t nrOfBytes);
   virtual ~Serializer();

   // Operators:
   void operator=(const Serializer &other);

   // Get/set:
   void *getData() const;
   void *getDataAtCurPos() const;
   uint64_t getNrOfBytes() const;

   // Serialization:
   void clear();
   void reset();  
   bool deserialize(std::string &text);
   bool deserialize(uint8_t &byte);
   bool deserialize(bool &_bool);
   bool deserialize(uint32_t &uint);
   bool deserialize(float &_float);
   bool deserialize(glm::vec3 &vec);
   bool deserialize(glm::vec4 &vec);
   bool deserialize(glm::mat4 &mat);
   bool deserialize(void *rawData, uint64_t nrOfBytes);   


///////////
private: //
///////////

   // Reserved:
   struct Reserved;
   std::unique_ptr<Reserved> reserved;
};


