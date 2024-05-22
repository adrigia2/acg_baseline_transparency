/**
 * @file		engine_texture.h
 * @brief	OpenGL texture 
 *
 * @author	Achille Peternier (achille.peternier@supsi.ch), (C) SUPSI
 */
#pragma once



/**
 * @brief Class for modeling a generic OpenGL texture. 
 */
class ENG_API Texture final : public Eng::Object, public Eng::Managed
{
//////////
public: //
//////////

   // Special values:
   static Texture empty;   


   /**
    * @brief Types of texture. 
    */
   enum class Type : uint32_t
   {
      none,

      // Texture levels:
      albedo,      
      normal,
      roughness,
      metalness,

      // Terminator:
      last
   };


   /**
    * @brief Types of texture formats.
    */
   enum class Format : uint32_t
   {
      none,

      // Uncompressed formats:
      r8g8b8a8,
      r8g8b8,      
      
      // Compressed formats:      
      r8g8b8a8_compressed,
      r8g8b8_compressed,
      r8g8_compressed,
      r8_compressed,

      // Depth maps:
      depth,

      // Terminator:
      last
   };
   

   // Const/dest:
	Texture();      
	Texture(Texture &&other);
   Texture(Texture const&) = delete;   
   Texture(const Eng::Bitmap &bitmap);
   virtual ~Texture();  

   // Default textures:
   static const Texture &getDefault(bool white = true);

   // Get/set:
   const Eng::Bitmap &getBitmap() const;
   Format getFormat() const;
   uint32_t getSizeX() const;
   uint32_t getSizeY() const;
   uint32_t getSizeZ() const;   
   uint32_t getOglHandle() const;
   uint64_t getOglBindlessHandle() const;

   // Bitmap:
   bool load(const Eng::Bitmap &bitmap);
   bool create(uint32_t sizeX, uint32_t sizeY, Format format);

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
   Texture(const std::string &name);

   // Get/set:
   void setBitmap(const Eng::Bitmap &bitmap);
   void setFormat(Format format);
   void setSizeX(uint32_t sizeX);
   void setSizeY(uint32_t sizeY);
   void setSizeZ(uint32_t sizeZ);

   // Internal memory manager:   
   bool makeResident();
};


