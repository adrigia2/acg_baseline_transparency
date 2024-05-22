/**
 * @file		engine_program.h
 * @brief	OpenGL program object
 *
 * @author	Achille Peternier (achille.peternier@supsi.ch), (C) SUPSI
 */
#pragma once



/**
 * @brief Class for modeling an OpenGL program.
 */
class ENG_API Program final : public Eng::Object, public Eng::Managed
{
//////////
public: //
//////////

   // Special values:
   static Program empty;


   /**
    * @brief Types of program.
    */
   enum class Type : uint32_t
   {
      none,

      // Programs:
      program,

      // Terminator:
      last
   };


   // Const/dest:
   Program();
   Program(Program &&other);
   Program(Program const &) = delete;
   ~Program();

   // Get/set:
   const Type getType() const;
   const uint32_t getNrOfShaders() const;
   const Eng::Shader &getShader(uint32_t id) const;   
   bool setFloat(const std::string &name, float value);
   bool setInt(const std::string &name, int32_t value);
   bool setUInt(const std::string &name, uint32_t value);
   bool setUInt64(const std::string &name, uint64_t value);
   bool setVec3(const std::string &name, const glm::vec3 &value);
   bool setVec4(const std::string &name, const glm::vec4 &value);
   bool setMat3(const std::string &name, const glm::mat3 &value);
   bool setMat4(const std::string &name, const glm::mat4 &value);

   // Building:
   bool build(std::initializer_list<std::reference_wrapper<Eng::Shader>> args);

   // Rendering methods:
   bool render(uint32_t value = 0, void *data = nullptr) const;
   static void reset();

   // Compute-only:
   bool compute(uint32_t sizeX, uint32_t sizeY = 1, uint32_t sizeZ = 1) const;
   bool wait() const;

   // Cache:
   static Program &getCached();

   // Managed:
   bool init() override;
   bool free() override;  


///////////
private: //
///////////

   // Cache:
   static std::reference_wrapper<Eng::Program> cache;

   // Reserved:
   struct Reserved;
   std::unique_ptr<Reserved> reserved;

   // Const/dest:
   Program(const std::string &name);

   // Get/set:
   int32_t getParamLocation(const std::string &name);   
};

