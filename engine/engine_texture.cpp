/**
 * @file		engine_texture.cpp
 * @brief	OpenGL texture
 *
 * @author	Achille Peternier (achille.peternier@supsi.ch), (C) SUPSI
 */



//////////////
// #INCLUDE //
//////////////

   // Main include:
   #include "engine.h"

   // OGL:      
   #include <GL/glew.h>
   #include <GLFW/glfw3.h>

   

////////////
// STATIC //
////////////

   // Special values:
   Eng::Texture Eng::Texture::empty("[empty]");   



/////////////////////////
// RESERVED STRUCTURES //
/////////////////////////

/**
 * @brief Texture reserved structure.
 */
struct Eng::Texture::Reserved
{ 
   std::reference_wrapper<const Eng::Bitmap> bitmap;
   Eng::Texture::Format format;
   glm::u32vec3 size;
   
   GLuint oglId;                    ///< OpenGL texture ID   
   GLuint64 oglBindlessHandle;      ///< GL_ARB_bindless_texture special handle
   GLuint oglInternalFormat;        ///< OpenGL internal format enum


   /**
    * Constructor. 
    */
   Reserved() : bitmap{ Eng::Bitmap::empty }, format{ Eng::Texture::Format::none }, size{ 0, 0, 1 },
                oglId{ 0 }, oglBindlessHandle{ 0 }, oglInternalFormat{ 0 }
   {}
};



///////////////////////////
// BODY OF CLASS Texture //
///////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Constructor.
 */
ENG_API Eng::Texture::Texture() : reserved(std::make_unique<Eng::Texture::Reserved>())
{		
   ENG_LOG_DETAIL("[+]");
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Constructor from bitmap.
 * @param bitmap working bitmap
 */
ENG_API Eng::Texture::Texture(const Eng::Bitmap &bitmap) : reserved(std::make_unique<Eng::Texture::Reserved>())
{
   ENG_LOG_DETAIL("[+]");
   load(bitmap);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Constructor with name.
 * @param name node name
 */
ENG_API Eng::Texture::Texture(const std::string &name) : Eng::Object(name), reserved(std::make_unique<Eng::Texture::Reserved>())
{	   
   ENG_LOG_DETAIL("[+]");
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Move constructor. 
 */
ENG_API Eng::Texture::Texture(Texture &&other) : Eng::Object(std::move(other)), Eng::Managed(std::move(other)), reserved(std::move(other.reserved))
{  
   ENG_LOG_DETAIL("[M]");
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Destructor.
 */
ENG_API Eng::Texture::~Texture()
{	
   ENG_LOG_DETAIL("[-]");
   if (reserved) // Because of the move constructor   
      this->free();   
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Get texture bitmap.
 * @return texture bitmap
 */
const Eng::Bitmap ENG_API &Eng::Texture::getBitmap() const
{	
   return reserved->bitmap.get();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Set texture bitmap.
 * @param bitmap bitmap
 */
void ENG_API Eng::Texture::setBitmap(const Eng::Bitmap &bitmap)
{	
   reserved->bitmap = bitmap;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Get texture format.
 * @return texture format
 */
Eng::Texture::Format ENG_API Eng::Texture::getFormat() const
{
   return reserved->format;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Set texture format.
 * @param format texture format
 */
void ENG_API Eng::Texture::setFormat(Eng::Texture::Format format)
{
   reserved->format = format;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Get texture size X.
 * @return texture size X
 */
uint32_t ENG_API Eng::Texture::getSizeX() const
{
   return reserved->size.x;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Set texture size X.
 * @param sizeX texture width
 */
void ENG_API Eng::Texture::setSizeX(uint32_t sizeX)
{
   reserved->size.x = sizeX;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Get texture size Y.
 * @return texture size Y
 */
uint32_t ENG_API Eng::Texture::getSizeY() const
{
   return reserved->size.y;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Set texture size Y.
 * @param sizeY texture height
 */
void ENG_API Eng::Texture::setSizeY(uint32_t sizeY)
{
   reserved->size.y = sizeY;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Get texture size Z.
 * @return texture size Z
 */
uint32_t ENG_API Eng::Texture::getSizeZ() const
{
   return reserved->size.z;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Set texture size Z.
 * @param sizeX texture depth
 */
void ENG_API Eng::Texture::setSizeZ(uint32_t sizeZ)
{
   reserved->size.z = sizeZ;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Return the GLuint texture ID. 
 * @return texture ID or 0 if not valid
 */
uint32_t ENG_API Eng::Texture::getOglHandle() const
{	
   return reserved->oglId;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Return the GLuint64 bindless texture ID.
 * @return binddless texture handle or 0 if not valid
 */
uint64_t ENG_API Eng::Texture::getOglBindlessHandle() const
{
   return reserved->oglBindlessHandle;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Create an OpenGL instance of the texture. 
 * @return TF
 */
bool ENG_API Eng::Texture::init()
{	
   if (this->Eng::Managed::init() == false)
      return false;

   // Free texture if already stored:
   if (reserved->oglBindlessHandle)
   {
      glMakeTextureHandleNonResidentARB(reserved->oglBindlessHandle);
      reserved->oglBindlessHandle = 0;
   }
   if (reserved->oglId)   
   {
	   glDeleteTextures(1, &reserved->oglId);
      reserved->oglId = 0;
   }   

	// Create it:		    
   glGenTextures(1, &reserved->oglId);           	

   // Done:   
   return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Makes the texture resident (can't be modified anymore).
 * @return TF
 */
bool ENG_API Eng::Texture::makeResident()
{
   // Sanity check:
   if (reserved->oglBindlessHandle)
   {
      ENG_LOG_ERROR("Texture already resident");
      return false;
   }

   // Bindless:   
   reserved->oglBindlessHandle = glGetTextureHandleARB(reserved->oglId);
   glMakeTextureHandleResidentARB(reserved->oglBindlessHandle);

   // Done:   
   return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Destroy an OpenGL instance. 
 * @return TF
 */
bool ENG_API Eng::Texture::free() 
{	
   if (this->Eng::Managed::free() == false)
      return false;

   // Free texture if stored:
   if (reserved->oglBindlessHandle)
   {
      glMakeTextureHandleNonResidentARB(reserved->oglBindlessHandle);
      reserved->oglBindlessHandle = 0;
   }
   if (reserved->oglId)   
   {      
	   glDeleteTextures(1, &reserved->oglId);
      reserved->oglId = 0;
   }   

   // Done:   
   return true;
}	 


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Returns a default texture (either fully white or black).
 * @param white when true (default), a white texture is returned, black otherwise
 * @return texture reference
 */
const Eng::Texture ENG_API &Eng::Texture::getDefault(bool white)
{	
   if (white)
   {
      uint8_t data[] = { 255, 255, 255, 255 };
      static Eng::Bitmap dfltWhiteBitmap(Eng::Bitmap::Format::r8g8b8a8, 1, 1, data);
      static Texture dfltWhiteTexture(dfltWhiteBitmap);
      return dfltWhiteTexture;
   }
   else // Black
   {
      uint8_t data[] = { 0, 0, 0, 0 };
      static Eng::Bitmap dfltBlackBitmap(Eng::Bitmap::Format::r8g8b8a8, 1, 1, data);
      static Texture dfltBlackTexture(dfltBlackBitmap);
      return dfltBlackTexture;
   }
}	 


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Load the content of the texture from the given bitmap.
 * @param bitmap bitmap
 * @return TF
 */
bool ENG_API Eng::Texture::load(const Eng::Bitmap &bitmap) 
{	
   // Safety net:
   if (bitmap == Eng::Bitmap::empty)
   {
      ENG_LOG_ERROR("Invalid params");
      return false;
   }      

   // Bind texture and copy content:   
	GLuint intFormat;
	GLuint extFormat;
	GLuint extType;
   GLuint nrOfComponents;
   Format _format = Format::none;
	switch (bitmap.getFormat())
	{
      //////////////////////////////////////
      case Eng::Bitmap::Format::r8g8b8a8: //		   
		   intFormat      = GL_RGBA8;
		   extFormat      = GL_RGBA;
		   extType        = GL_UNSIGNED_BYTE;
         nrOfComponents = 4;
         _format        = Format::r8g8b8a8;
		   break;	      

      ////////////////////////////////////
      case Eng::Bitmap::Format::r8g8b8: //         
         intFormat      = GL_RGB8;
         extFormat      = GL_RGB;
         extType        = GL_UNSIGNED_BYTE;
         nrOfComponents = 3;
         _format        = Format::r8g8b8;
         break;      				

      /////////////////////////////////////////////////
      case Eng::Bitmap::Format::r8g8b8a8_compressed: //		   
		   intFormat      = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
		   extFormat      = GL_RGBA;
		   extType        = GL_UNSIGNED_BYTE;
         nrOfComponents = 4;
         _format        = Format::r8g8b8a8_compressed;
		   break;	      

      ///////////////////////////////////////////////
      case Eng::Bitmap::Format::r8g8b8_compressed: //		   
         intFormat      = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;		   
		   extFormat      = GL_RGB;
		   extType        = GL_UNSIGNED_BYTE;
         nrOfComponents = 3;
         _format        = Format::r8g8b8_compressed;
		   break;	      

      /////////////////////////////////////////////
      case Eng::Bitmap::Format::r8g8_compressed: //		   
         intFormat      = GL_COMPRESSED_RG_RGTC2;		   
		   extFormat      = GL_RG;
		   extType        = GL_UNSIGNED_BYTE;
         nrOfComponents = 2;
         _format        = Format::r8g8_compressed;
		   break;	      

      ///////////////////////////////////////////
      case Eng::Bitmap::Format::r8_compressed: //		   
         intFormat      = GL_COMPRESSED_RED_RGTC1;		   
		   extFormat      = GL_R;
		   extType        = GL_UNSIGNED_BYTE;
         nrOfComponents = 1;
         _format        = Format::r8_compressed;
		   break;	      

		///////////
      default: //
         ENG_LOG_ERROR("Unexpected bitmap type");
			return false;			
	}

   // Init texture:
   this->Eng::Texture::init();

	// Create it:		              
   const GLuint oglId = this->getOglHandle();
   glBindTexture(GL_TEXTURE_2D, oglId);   
   if (bitmap.getNrOfLevels() > 1)
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, bitmap.getNrOfLevels());   
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, Eng::Base::getInstance().isBindlessSupported() ? 16 : 1);    
   
   // Load data:   
   for (uint32_t side = 0; side < bitmap.getNrOfSides(); side++)
      for (uint32_t c = 0; c < bitmap.getNrOfLevels(); c++)
      {
         ENG_LOG_DEBUG("Type: 2D, Level: %d/%d, IntFormat: 0x%x, x: %u, y: %u", c + 1, bitmap.getNrOfLevels(), intFormat, bitmap.getSizeX(c), bitmap.getSizeY(c)); 
         switch (_format)
         {
            // Compressed:
            case Format::r8g8b8a8_compressed:
            case Format::r8g8b8_compressed:
            case Format::r8g8_compressed:
            case Format::r8_compressed:
               glCompressedTexImage2D(GL_TEXTURE_2D, c, intFormat, bitmap.getSizeX(c), bitmap.getSizeY(c), 0, bitmap.getNrOfBytes(c), bitmap.getData(c));  
               break;

            // Uncompressed:
            default:
               glTexImage2D(GL_TEXTURE_2D, c, intFormat, bitmap.getSizeX(c), bitmap.getSizeY(c), 0, extFormat, extType, bitmap.getData(c));  
         }         
      }

   if (bitmap.getNrOfLevels() <= 1)
      glGenerateMipmap(GL_TEXTURE_2D); 

   // Resident (if supported):
   if (Eng::Base::getInstance().isBindlessSupported())
      this->Eng::Texture::makeResident();

   // Done:   
   this->setBitmap(bitmap);
   this->setFormat(_format);
   reserved->oglInternalFormat = intFormat;
   this->setSizeX(bitmap.getSizeX(0));
   this->setSizeY(bitmap.getSizeY(0));
   return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	 
/** 
 * Allocate memory and initialize an empty texture. 
 * @param sizeX texture width
 * @param sizeY texture height  
 * @param format pixel layout
 * @return TF
 */	
bool ENG_API Eng::Texture::create(uint32_t sizeX, uint32_t sizeY, Format format)
{ 
	// Safety net:
	if (sizeX == 0 || sizeY == 0 || format == Format::none)
	{
      ENG_LOG_ERROR("Invalid params");
		return false;
	}	   
   
   // Bind texture and copy content:   
	GLuint intFormat;
	GLuint extFormat;
	GLuint extType;
   GLuint nrOfComponents;
	switch (format)
	{
      ///////////////////////
      case Format::r8g8b8: //    
         intFormat      = GL_RGB8;
         extFormat      = GL_RGB;
         extType        = GL_UNSIGNED_BYTE;
         nrOfComponents = 3;
         break;
      		
		/////////////////////////
      case Format::r8g8b8a8: //
		   intFormat      = GL_RGBA8;
		   extFormat      = GL_RGBA;
		   extType        = GL_UNSIGNED_BYTE;
         nrOfComponents = 4;
		   break;	      

      //////////////////////
      case Format::depth: //
         intFormat = GL_DEPTH_COMPONENT32F;
         extFormat = GL_DEPTH_COMPONENT;
         extType = GL_FLOAT;
         nrOfComponents = 1;
         break;

		///////////
      default: //
         ENG_LOG_ERROR("Unexpected format type");
			return false;			
	}  

   // Init texture:
   this->Eng::Texture::init();   

	// Create it:		    
   const GLuint oglId = this->getOglHandle();
   glBindTexture(GL_TEXTURE_2D, oglId);   	      	
   glTexImage2D(GL_TEXTURE_2D, 0, intFormat, sizeX, sizeY, 0, extFormat, extType, nullptr);         
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);   
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); 
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);   
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);     
   if (format == Format::depth)
   {
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
      float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
      glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);      
   }
   
   // Resident (if supported):
   if (Eng::Base::getInstance().isBindlessSupported())
      this->Eng::Texture::makeResident();
   	
	// Done:
   this->setFormat(format);
   reserved->oglInternalFormat = intFormat;
   this->setSizeX(sizeX);
   this->setSizeY(sizeY);
	return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Binds image for imagestore operations.
 * @param location location to bind the texture on 
 * @return TF
 */
bool ENG_API Eng::Texture::bindImage(uint32_t location)
{     
   glBindImageTexture(location, reserved->oglId, 0, GL_FALSE, 0, GL_READ_WRITE, reserved->oglInternalFormat);      
   return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Empty rendering method. Bad sign if you read this. 
 * @param value generic value
 * @param data generic pointer to any kind of data
 * @return TF
 */
bool ENG_API Eng::Texture::render(uint32_t value, void *data) const
{	   
   Eng::Program &program = Eng::Program::getCached();
   
   // Bindless: 
   if (Eng::Base::getInstance().isBindlessSupported())
   {
      std::string texLevel = "texture" + std::to_string(value);
      program.setUInt64(texLevel, this->getOglBindlessHandle());
   }
   else // ...or old-school:
   {      
      glBindTextures(value, 1, &reserved->oglId);
   }

   // Done:
   return true;
}
