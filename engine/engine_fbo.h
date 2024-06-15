/**
 * @file		engine_fbo.h
 * @brief	OpenGL framebuffer object
 *
 * @author	Achille Peternier (achille.peternier@supsi.ch), (C) SUPSI
 */
#pragma once



/**
 * @brief Class for modeling an OpenGL framebuffer. 
 */
class ENG_API Fbo final : public Eng::Object, public Eng::Managed
{
//////////
public: //
//////////

   // Special values:
   static Fbo empty;   


   /**
    * Attachment.
    */
   struct ENG_API Attachment
   {
      // Special values:
      static Attachment empty;   


      /**
       * @brief Types of framebuffer attachments. 
       */
      enum class Type : uint32_t
      {
         none,

         // Texture attachments:      
         color_texture,
         depth_texture,      

         // Render buffer attachments:
         // color_buffer,   // Useless, not implemented
         depth_buffer,         

         // Terminator:
         last
      };

      Type type;                    ///< Type of attachment
      glm::u32vec2 size;            ///< Attachment size
      union
      {
         std::reference_wrapper<const Eng::Texture> texture;   ///< Texture reference
         uint64_t data;                                        ///< Buffer ID (stored as uint64 instead of GLuint)   
      };


      /**
       * Constructor.
       */
      Attachment();
   };


   // Const/dest:
	Fbo();      
	Fbo(Fbo &&other);
   Fbo(Fbo const&) = delete;   
   ~Fbo();         

   // Get/set:
   uint32_t getNrOfAttachments() const;   
   uint32_t getSizeX() const;
   uint32_t getSizeY() const;

   // Attachments:   
   bool attachTexture(const Eng::Texture &texture, uint32_t level = 0, uint32_t side = 0);   
   // bool attachColorBuffer(uint32_t sizeX, uint32_t sizeY); // Useless, not implemented
   bool attachDepthBuffer(uint32_t sizeX, uint32_t sizeY);      
   bool validate() const;

   // Rendering methods:
   bool render(uint32_t value = 0, void *data = nullptr) const;
   static void reset(uint32_t viewportSizeX, uint32_t viewportSizeY);
   bool blit(uint32_t viewportSizeX, uint32_t viewportSizeY, bool invertOrder=false) const;

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
   Fbo(const std::string &name);

   // Get/set:
   uint32_t getOglHandle() const;

   // Management:
   bool updateMrtCache();
};
