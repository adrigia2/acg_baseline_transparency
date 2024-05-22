/**
 * @file		engine_shader.h
 * @brief	OpenGL shader object
 *
 * @author	Achille Peternier (achille.peternier@supsi.ch), (C) SUPSI
 */
#pragma once



/**
 * @brief Class for modeling a generic shader.
 */
class ENG_API Shader final : public Eng::Object, public Eng::Managed
{
//////////
public: //
//////////

   // Special values:
   static Shader empty;

   // Consts:
   static constexpr uint32_t maxLogSize =  4096;              ///< Max shader compiler log size in bytes


   /**
    * @brief Types of shader.
    */
   enum class Type : uint32_t
   {
      none,

      // Shaders:
      vertex,
      tessellation_ctrl,
      tessellation_eval,
      geometry,
      fragment,
      compute,

      // Terminator:
      last
   };


   // Const/dest:
   Shader();
   Shader(Shader &&other);
   Shader(Shader const &) = delete;
   ~Shader();   
   
   // Get/set:
   const Type getType() const;
   const std::string &getCode() const;
   uint32_t getOglHandle() const;

   // Accessing data:
   bool load(Type kind, const std::string &code);

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
   Shader(const std::string &name);
  
   // Workaround for disabling the unneeded rendering method:
   using Object::render;
};


