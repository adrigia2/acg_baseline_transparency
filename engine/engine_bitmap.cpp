/**
 * @file		engine_bitmap.cpp
 * @brief	Bitmap support (through DDS files)
 *
 * @author	Achille Peternier (achille.peternier@supsi.ch), (C) SUPSI
 */



//////////////
// #INCLUDE //
//////////////

   // Main include:
   #include "engine.h"

   // DDS support:
   #include "dds.h"   

   // C/C++:
   #include <algorithm>

   

////////////
// STATIC //
////////////

   // Special values:
   Eng::Bitmap Eng::Bitmap::empty("[empty]");   



/////////////////////////
// RESERVED STRUCTURES //
/////////////////////////

/**
 * @brief Bitmap reserved structure.
 */
struct Eng::Bitmap::Reserved
{ 
   /**
    * @brief Bitmap layer.
    */
   struct Layer
   {      
      std::vector<uint8_t> data;       ///< Image raw data
      glm::u32vec2 size;               ///< Layer size


      /**
       * Constructor.
       */
      Layer() : size{ 0, 0 }
      {}
   };

   Eng::Bitmap::Format format;      ///< Image format
   std::vector<Layer> layer;        ///< Bitmap layers;
   uint32_t nrOfLevels;             ///< Number of levels (mipmaps)
   uint32_t nrOfSides;              ///< Number of sides (faces)
   float compressionFactor;         ///< Compression factor


   /**
    * Constructor. 
    */
   Reserved() : format{ Eng::Bitmap::Format::none }, 
                nrOfLevels{ 0 }, nrOfSides{ 0 }, 
                compressionFactor{ 1.0f }
   {}
};



//////////////////////////
// BODY OF CLASS Bitmap //
//////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Constructor.
 */
ENG_API Eng::Bitmap::Bitmap() : reserved(std::make_unique<Eng::Bitmap::Reserved>())
{		
   ENG_LOG_DETAIL("[+]");
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Constructor.
 */
ENG_API Eng::Bitmap::Bitmap(Format format, uint32_t sizeX, uint32_t sizeY, uint8_t *data) : reserved(std::make_unique<Eng::Bitmap::Reserved>())
{
   ENG_LOG_DETAIL("[+]");
   load(format, sizeX, sizeY, data);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Constructor with name.
 * @param name node name
 */
ENG_API Eng::Bitmap::Bitmap(const std::string &name) : Eng::Object(name), reserved(std::make_unique<Eng::Bitmap::Reserved>())
{	   
   ENG_LOG_DETAIL("[+]");
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Move constructor. 
 */
ENG_API Eng::Bitmap::Bitmap(Bitmap &&other) : Eng::Object(std::move(other)), reserved(std::move(other.reserved))
{  
   ENG_LOG_DETAIL("[M]");
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Destructor.
 */
ENG_API Eng::Bitmap::~Bitmap()
{	
   ENG_LOG_DETAIL("[-]");
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Get bitmap format.
 * @return bitmap format (as Format enum)
 */
Eng::Bitmap::Format ENG_API Eng::Bitmap::getFormat() const
{	
   return reserved->format;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Get number of sides.
 * @return number of sides
 */
uint32_t ENG_API Eng::Bitmap::getNrOfSides() const
{	
   return reserved->nrOfSides;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Get number of levels.
 * @return number of levels
 */
uint32_t ENG_API Eng::Bitmap::getNrOfLevels() const
{	
   return reserved->nrOfLevels;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Get compression factor.
 * @return compression factor
 */
float ENG_API Eng::Bitmap::getCompressionFactor() const
{	
   return reserved->compressionFactor;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Get color depth in bytes.
 * @return color depth or 0 on error
 */
uint32_t ENG_API Eng::Bitmap::getColorDepth() const
{	
   switch (reserved->format)   
   {
      case Format::r8_compressed:
         return 1;

      case Format::r8g8_compressed:
         return 2;

      case Format::r8g8b8:
      case Format::r8g8b8_compressed:
         return 3;

      case Format::r8g8b8a8:
      case Format::r8g8b8a8_compressed:
         return 4;

      default:
         ENG_LOG_ERROR("Invalid value");
         return 0;
   }
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Get width in pixels of the specific level/side.
 * @param level
 * @param side
 * @return width in pixels or 0 on error
 */
uint32_t ENG_API Eng::Bitmap::getSizeX(uint32_t level, uint32_t side) const
{	
   // Safet net:
   if (reserved->layer.empty() || level >= reserved->nrOfLevels || side >= reserved->nrOfSides)
   {   
      ENG_LOG_ERROR("Invalid params");
      return 0;
   }

   return reserved->layer[side * reserved->nrOfLevels + level].size.x;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Get height in pixels of the specific level/side.
 * @param level
 * @param side
 * @return height in pixels or 0 on error
 */
uint32_t ENG_API Eng::Bitmap::getSizeY(uint32_t level, uint32_t side) const
{	
   // Safet net:
   if (reserved->layer.empty() || level >= reserved->nrOfLevels || side >= reserved->nrOfSides)
   {   
      ENG_LOG_ERROR("Invalid params");
      return 0;
   }

   return reserved->layer[side * reserved->nrOfLevels + level].size.y;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Get a pointer to the data of a specific level/side.
 * @param level
 * @param side
 * @return pointer to data
 */
uint8_t ENG_API *Eng::Bitmap::getData(uint32_t level, uint32_t side) const
{	
   // Safet net:
   if (reserved->layer.empty() || level >= reserved->nrOfLevels || side >= reserved->nrOfSides)
   {   
      ENG_LOG_ERROR("Invalid params");
      return nullptr;
   }

   return reserved->layer[side * reserved->nrOfLevels + level].data.data();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Returns the size in bytes of the given level/side.
 * @param level
 * @param side
 * @return size in bytes
 */
uint32_t ENG_API Eng::Bitmap::getNrOfBytes(uint32_t level, uint32_t side) const
{	
   // Safet net:
   if (reserved->layer.empty() || level >= reserved->nrOfLevels || side >= reserved->nrOfSides)
   {   
      ENG_LOG_ERROR("Invalid params");
      return 0;
   }

   return static_cast<uint32_t>(reserved->layer[side * reserved->nrOfLevels + level].data.size());
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Load image from memory.
 * @param format image format
 * @param sizeX width in pixels
 * @param sizeY height in pixels
 * @param data pointer to the image data
 * @return TF
 */
bool ENG_API Eng::Bitmap::load(Format format, uint32_t sizeX, uint32_t sizeY, uint8_t *data)
{
   // Safety net:
   if (data == nullptr)
   {
      ENG_LOG_ERROR("Invalid params");
      return false;
   }

   // Image size:
   uint32_t colorDepth = 0;
   switch (format)   
   {
      case Format::r8g8b8: colorDepth = 3; break;
      case Format::r8g8b8a8: colorDepth = 4; break;
      default:
         ENG_LOG_ERROR("Invalid format");
         return false;
   }
   uint64_t size = (uint64_t) sizeX * (uint64_t) sizeY * (uint64_t) colorDepth;

   // Free previous image?
   reserved->layer.clear();  
   
   // Force single image:
   reserved->format = format;
   reserved->nrOfSides = 1;
   reserved->nrOfLevels = 1; 

   // Allocate and populate layer:   
   Reserved::Layer l;   
   l.size.x = sizeX;
   l.size.y = sizeY;
   l.data.resize(size);         
   memcpy(l.data.data(), data, size); 
   
   // Store layer:
   reserved->layer.push_back(l);   
   
   // Done:   
   this->setName(std::string("fromMemory"));
   return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Load image from a .dds file.
 * @param filename DDS file name
 * @return TF
 */
bool ENG_API Eng::Bitmap::load(const std::string &filename)
{
   // Safety net:
   if (filename.empty())
   {
      ENG_LOG_ERROR("Invalid params");
      return false;
   }
  
   // Free previous image?
   reserved->layer.clear();  

   // Get file size:
   FILE *dat = fopen(filename.c_str(), "rb");
   if (dat == nullptr)
   {
      ENG_LOG_ERROR("File '%s' not found", filename.c_str());
      return false;
   }
   fseek(dat, 0, SEEK_END);
   uint64_t filesize = ftell(dat);
   fseek(dat, 0, SEEK_SET);

   // Copy file to memory:
   std::vector<uint8_t> data(filesize);
   uint8_t *position = data.data();
   if (fread(position, sizeof(uint8_t), filesize, dat) != filesize)
   {
      ENG_LOG_ERROR("File '%s' damaged", filename.c_str());
      fclose(dat);
      return false;
   }
   fclose(dat);

   // Check header:   
   uint32_t magicNumber;
   memcpy(&magicNumber, position, sizeof(uint32_t)); position += sizeof(uint32_t);
   if (magicNumber != DDS_MAGICNUMBER)
   {
      ENG_LOG_ERROR("File '%s' is not a valid DDS", filename.c_str());      
      return false;
   }

   // Get header:
   DDS_HEADER *header = reinterpret_cast<DDS_HEADER *> (position); position += sizeof(DDS_HEADER);
   reserved->nrOfLevels = header->dwMipMapCount;

   // Cubemap (old format)?
   reserved->nrOfSides = 1;
   if (header->dwCaps2 & DDSCAPS2_CUBEMAP)
   {
      ENG_LOG_DEBUG("Image is a cubemap");

      // Check completeness (only 6-sided cubemaps are supported):
      bool complete = true;
      if (!(header->dwCaps2 & DDSCAPS2_CUBEMAP_POSITIVEX)) complete = false;
      if (!(header->dwCaps2 & DDSCAPS2_CUBEMAP_POSITIVEY)) complete = false;
      if (!(header->dwCaps2 & DDSCAPS2_CUBEMAP_POSITIVEZ)) complete = false;
      if (!(header->dwCaps2 & DDSCAPS2_CUBEMAP_NEGATIVEX)) complete = false;
      if (!(header->dwCaps2 & DDSCAPS2_CUBEMAP_NEGATIVEY)) complete = false;
      if (!(header->dwCaps2 & DDSCAPS2_CUBEMAP_NEGATIVEZ)) complete = false;
      if (!complete)
      {
         ENG_LOG_ERROR("File '%s' is an incomplete cubemap", filename.c_str());         
         return false;
      }
      reserved->nrOfSides = 6;
   }  

   // Check format:
   char fourCC[5];
   memcpy(fourCC, &header->ddspf.dwFourCC, 4);
   fourCC[4] = '\0';

   ENG_LOG_DEBUG("File fourCC: '%s'", fourCC);
   if (strcmp(fourCC, "DXT1") == 0)   
      reserved->format = Eng::Bitmap::Format::r8g8b8_compressed;      
   else
      if (strcmp(fourCC, "DXT5") == 0)      
         reserved->format = Eng::Bitmap::Format::r8g8b8a8_compressed;         
      else
         if (strcmp(fourCC, "ATI1") == 0)         
            reserved->format = Eng::Bitmap::Format::r8_compressed;            
         else
            if (strcmp(fourCC, "ATI2") == 0)            
               reserved->format = Eng::Bitmap::Format::r8g8_compressed;                           
            else
               if (strcmp(fourCC, "DX10") == 0)
               {
                  // Get header10:
                  DDS_HEADER10 *header10 = reinterpret_cast<DDS_HEADER10 *> (position); position += sizeof(DDS_HEADER10);

                  // Cube map (new format)?
                  ENG_LOG_DEBUG("Array: %u", header10->arraySize);
                  if (header10->arraySize == 6)
                  {
                     ENG_LOG_DEBUG("Image is a cubemap");
                     reserved->nrOfSides = header10->arraySize;
                  }

                  // Check format:
                  switch (header10->dxgiFormat)
                  {
                     case DXGI_FORMAT_BC1_UNORM:
                        reserved->format = Eng::Bitmap::Format::r8g8b8_compressed;                      
                        break;

                     case DXGI_FORMAT_BC3_UNORM:
                        reserved->format = Eng::Bitmap::Format::r8g8b8a8_compressed;                        
                        break;

                     default:
                        ENG_LOG_ERROR("File '%s' uses an unsupported DX10 compression format", filename.c_str());                                                
                        return false;
                  }
               }
               else
               {
                  ENG_LOG_ERROR("File '%s' uses an unsupported compression format", filename.c_str());                  
                  return false;
               }   
   
   switch (reserved->format)
   {
      case Eng::Bitmap::Format::r8_compressed:         reserved->compressionFactor = 0.5f; break;
      case Eng::Bitmap::Format::r8g8_compressed:       reserved->compressionFactor = 1.0f; break;
      case Eng::Bitmap::Format::r8g8b8_compressed:     reserved->compressionFactor = 0.5f; break;
      case Eng::Bitmap::Format::r8g8b8a8_compressed:   reserved->compressionFactor = 1.0f; break;
   }

   // Allocate and populate layers:   
   Reserved::Layer l;   
   for (uint32_t s = 0; s < reserved->nrOfSides; s++)
   {  
      uint32_t sizeX = header->dwWidth;
      uint32_t sizeY = header->dwHeight;
      for (uint32_t c = 0; c < reserved->nrOfLevels; c++)
      {  
         // Store layer:
         reserved->layer.push_back(l);
         Reserved::Layer &curLayer = reserved->layer.back();

         curLayer.size.x = sizeX;
         curLayer.size.y = sizeY;
         uint32_t levelSize = (uint32_t) (reserved->compressionFactor * sizeX * sizeY);
         if (sizeX < 4)
            levelSize = (uint32_t) (reserved->compressionFactor * sizeX * 2 * sizeY);
         if (reserved->compressionFactor == 0.5f && levelSize < 8)
            levelSize = 8;
         if (reserved->compressionFactor == 1.0f && levelSize < 16)
            levelSize = 16;
         curLayer.data.resize(levelSize);         
         memcpy(curLayer.data.data(), position, levelSize); position += levelSize;

         ENG_LOG_DEBUG("Mipmap: %u, %ux%u, %u bytes", c, sizeX, sizeY, levelSize);

         // Update values:
         if (sizeX > 1)
            sizeX /= 2;
         if (sizeY > 1)
            sizeY /= 2;
      }
   }   
   
   // Done:
   this->setName(filename);
   return true;
}
