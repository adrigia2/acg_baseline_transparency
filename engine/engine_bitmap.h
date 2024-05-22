/**
 * @file		engine_bitmap.h
 * @brief	Bitmap support (through DDS files)
 *
 * @author	Achille Peternier (achille.peternier@supsi.ch), (C) SUPSI
 */
#pragma once



/**
 * @brief Class for modeling a generic bitmap. 
 */
class ENG_API Bitmap final : public Eng::Object
{
//////////
public: //
//////////

   // Special values:
   static Bitmap empty;   

   
   /**
    * @brief Types of bitmap format. 
    */
   enum class Format : uint32_t
   {
      none,

      // Uncompressed formats:
      r8g8b8a8,
      r8g8b8,      

      // Compressed:
      r8g8b8a8_compressed,
      r8g8b8_compressed,      
      r8g8_compressed,
      r8_compressed,

      // Terminator:
      last
   };


   // Const/dest:
	Bitmap();      
	Bitmap(Bitmap &&other);
   Bitmap(Bitmap const&) = delete;   
   Bitmap(Format format, uint32_t sizeX, uint32_t sizeY, uint8_t *data);
   virtual ~Bitmap();      

   // Get/set:
   Format getFormat() const;
   uint32_t getNrOfSides() const;
   uint32_t getNrOfLevels() const;
   uint32_t getColorDepth() const;
   uint32_t getSizeX(uint32_t level = 0, uint32_t side = 0) const;
   uint32_t getSizeY(uint32_t level = 0, uint32_t side = 0) const;
   uint32_t getNrOfBytes(uint32_t level = 0, uint32_t side = 0) const;
   uint8_t *getData(uint32_t level = 0, uint32_t side = 0) const;   
   float getCompressionFactor() const;

   // Loaders:
   bool load(const std::string &filename);
   bool load(Format format, uint32_t sizeX, uint32_t sizeY, uint8_t *data);


///////////
private: //
///////////

   // Reserved:
   struct Reserved;           
   std::unique_ptr<Reserved> reserved;			

   // Const/dest:
   Bitmap(const std::string &name);
};
