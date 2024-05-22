/**
 * @file		engine_object.h
 * @brief	Generic object properties
 *
 * @author	Achille Peternier (achille.peternier@supsi.ch), (C) SUPSI
 */
#pragma once



/**
 * @brief Class for modeling a generic, base object. This class is inherited by most of the engine classes.
 */
class ENG_API Object
{
//////////
public: //
//////////

   // Special values:
   static Object empty;


   /**
   * @brief Various flags.
   */
   enum class Flag : uint32_t
   {
      none = 0,
      upload = 1,    ///< Enable to only upload data

      // Terminator:
      last
   };


   // Const/dest:
   Object();
   Object(Object &&other);
   Object(Object const &) = delete;
   virtual ~Object();

   // Operators:
   void operator=(Object const &) = delete;
   bool operator==(const Object &rhs) const;
   bool operator!=(const Object &rhs) const;

   // General:
   void setName(const std::string &name);
   const std::string &getName() const;
   uint32_t getId() const;
   bool isDirty() const;
   void setDirty(bool dirty) const;

   // Statistics:
   static int32_t getNrOfObjects();

   // Rendering methods:
   virtual bool render(uint32_t value = 0, void *data = nullptr) const;


/////////////
protected: //
/////////////

   // Reserved:
   struct Reserved;
   std::unique_ptr<Reserved> reserved;

   // Const/dest:
   Object(const std::string &name);
};

