/**
 * @file		engine_serializer.cpp
 * @brief	Basic generic serializer
 *
 * @author	Achille Peternier (achille.peternier@supsi.ch), (C) SUPSI
 */



//////////////
// #INCLUDE //
//////////////

   // Main include:
   #include "engine.h"

   // C/C++:
   #include <iterator>



////////////
// STATIC //
////////////

   // Special values:
   Eng::Serializer Eng::Serializer::empty;



/////////////////////////
// RESERVED STRUCTURES //
/////////////////////////

/**
 * @brief Serializer reserved structure.
 */
struct Eng::Serializer::Reserved
{
   uint64_t position;
   uint64_t nrOfBytes;
   std::vector<uint8_t> data;


   /**
    * Constructor.
    */
   Reserved() : position{ 0 }, nrOfBytes{ 0 }
   {}
};



//////////////////////////////
// BODY OF CLASS Serializer //
//////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Constructor.
 */
ENG_API Eng::Serializer::Serializer() : reserved(std::make_unique<Eng::Serializer::Reserved>())
{
   ENG_LOG_DETAIL("[+]");
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Copy constructor.
 */
ENG_API Eng::Serializer::Serializer(const Serializer &other) : reserved(std::make_unique<Eng::Serializer::Reserved>())
{   
   ENG_LOG_DETAIL("[+]");
   *reserved = *other.reserved;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Constructor from serialized data.
 * @param rawData pointer to serialized data (nullptr allocates empty space)
 * @param nrOfBytes size of the serialized data
 */
ENG_API Eng::Serializer::Serializer(const void *rawData, uint64_t nrOfBytes) : reserved(std::make_unique<Eng::Serializer::Reserved>())
{
   ENG_LOG_DETAIL("[+]");
   reserved->nrOfBytes = nrOfBytes;
   if (rawData == nullptr)
      reserved->data.resize(nrOfBytes);
   else
   {
      const uint8_t *ptr = static_cast<const uint8_t *>(rawData);
      std::copy(ptr, ptr + nrOfBytes, std::back_inserter(reserved->data));
   }
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Destructor.
 */
ENG_API Eng::Serializer::~Serializer()
{
   ENG_LOG_DETAIL("[-]");
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Copy assignment.
 */
void ENG_API Eng::Serializer::operator=(const Serializer &other)
{   
   *reserved = *other.reserved;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Pointer to serialized data.
 * @return pointer to serialized data
 */
void ENG_API *Eng::Serializer::getData() const
{
   reserved->data.shrink_to_fit();
   return static_cast<void *>(reserved->data.data());   
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Pointer to serialized data at the current deserializing position.
 * @return pointer to serialized data at the current position, or nullptr if no more data left
 */
void ENG_API *Eng::Serializer::getDataAtCurPos() const
{
   if (reserved->position >= reserved->nrOfBytes)
      return nullptr;

   reserved->data.shrink_to_fit();
   return static_cast<void *>(reserved->data.data() + reserved->position);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Returns the number of bytes stored in the serializer.
 * @return number of bytes stored
 */
uint64_t ENG_API Eng::Serializer::getNrOfBytes() const
{
   return reserved->nrOfBytes;   
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Resets the internal data. 
 */
void ENG_API Eng::Serializer::reset()
{  
   reserved->position = 0;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Clears the internal data.
 */
void ENG_API Eng::Serializer::clear()
{
   reserved->data.clear();
   reserved->position = 0;
   reserved->nrOfBytes = 0;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Deserializes a string.
 * @param text string to deserialize
 * @return TF
 */
bool ENG_API Eng::Serializer::deserialize(std::string &text)
{ 
   uint32_t size = (uint32_t) strlen((char *)(&reserved->data[reserved->position]));   
   if (reserved->position + size > reserved->nrOfBytes)
   {
      ENG_LOG_ERROR("Corrupted serialization");
      return false;
   }
   text.resize(size);
   deserialize(text.data(), size);
   reserved->position++;

   // Done:
   return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Deserializes a byte.
 * @param byte byte to deserialize
 * @return TF
 */
bool ENG_API Eng::Serializer::deserialize(uint8_t &byte)
{
   deserialize(&byte, sizeof(uint8_t));

   // Done:
   return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Deserializes a boolean.
 * @param _bool boolean to deserialize
 * @return TF
 */
bool ENG_API Eng::Serializer::deserialize(bool &_bool)
{
   deserialize(&_bool, sizeof(bool));

   // Done:
   return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Deserializes a uint.
 * @param uint unsigned int to deserialize
 * @return TF
 */
bool ENG_API Eng::Serializer::deserialize(uint32_t &uint)
{
   deserialize(&uint, sizeof(uint32_t));

   // Done:
   return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Deserializes a float.
 * @param _float float to deserialize
 * @return TF
 */
bool ENG_API Eng::Serializer::deserialize(float &_float)
{
   deserialize(&_float, sizeof(float));

   // Done:
   return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Deserializes a vec3.
 * @param vec vec3 to deserialize
 * @return TF
 */
bool ENG_API Eng::Serializer::deserialize(glm::vec3 &vec)
{   
   deserialize(&vec, sizeof(glm::vec3));   

   // Done:
   return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Deserializes a vec4.
 * @param vec vec4 to deserialize
 * @return TF
 */
bool ENG_API Eng::Serializer::deserialize(glm::vec4 &vec)
{
   deserialize(&vec, sizeof(glm::vec4));

   // Done:
   return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Deserializes a mat4.
 * @param mat mat4 to deserialize
 * @return TF
 */
bool ENG_API Eng::Serializer::deserialize(glm::mat4 &mat)
{
   deserialize(&mat, sizeof(glm::mat4));

   // Done:
   return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Deserializes a series of raw bytes.
 * @param rawData pointer to data
 * @param nrOfBytes number of bytes
 * @return TF
 */
bool ENG_API Eng::Serializer::deserialize(void *rawData, uint64_t nrOfBytes)
{
   // Safet net:
   if (rawData == nullptr)
   {
      ENG_LOG_ERROR("Invalid params");
      return false;
   }
   if (reserved->position + nrOfBytes > reserved->nrOfBytes)
   {
      ENG_LOG_ERROR("Buffer overflow");
      return false;
   }

   // Increase and store:   
   memcpy(rawData, &reserved->data[reserved->position], nrOfBytes);
   reserved->position += nrOfBytes;

   // Done:
   return true;
}
