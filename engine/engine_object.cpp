/**
 * @file		engine_object.cpp
 * @brief	Basic generic object properties
 *
 * @author	Achille Peternier (achille.peternier@supsi.ch), (C) SUPSI
 */



//////////////
// #INCLUDE //
//////////////

   // Main include:
   #include "engine.h"



////////////
// STATIC //
////////////

   // Special values:
   Eng::Object Eng::Object::empty("[empty]");

   // Parity check and counters:
   static int32_t counter = 0;
   static uint32_t idCounter = 0;



/////////////////////////
// RESERVED STRUCTURES //
/////////////////////////

/**
 * @brief Object reserved structure.
 */
struct Eng::Object::Reserved
{
   // General:
   std::string name;                         ///< Name
   uint32_t id;                              ///< UID
   bool dirty;                               ///< Object needs update  


   /**
    * Constructor.
    */
   Reserved() : name{ "[none]" }, id{ idCounter++ }, dirty{ true }
   {
      counter++;
   }

   /**
    * Destructor.
    */
   ~Reserved()
   {
      counter--;
   }
};



//////////////////////////
// BODY OF CLASS Object //
//////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Constructor.
 */
ENG_API Eng::Object::Object() : reserved(std::make_unique<Eng::Object::Reserved>())
{
   ENG_LOG_DETAIL("[+]");
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Constructor with name.
 * @param name node name
 */
ENG_API Eng::Object::Object(const std::string &name) : Object()
{
   reserved->name = name;

   ENG_LOG_DETAIL("[+]");
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Move constructor.
 */
ENG_API Eng::Object::Object(Object &&other) : reserved(std::move(other.reserved))
{
   ENG_LOG_DETAIL("[M]");
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Destructor.
 */
ENG_API Eng::Object::~Object()
{
   ENG_LOG_DETAIL("[-]");
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Operator == override.
 */
bool ENG_API Eng::Object::operator==(const Object &rhs) const
{
   if (this->reserved == rhs.reserved)
      return true;
   else
      return false;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Operator != override.
 */
bool ENG_API Eng::Object::operator!=(const Object &rhs) const
{
   if (this->reserved != rhs.reserved)
      return true;
   else
      return false;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Set node name.
 * @param name node name
 */
void ENG_API Eng::Object::setName(const std::string &name)
{
   // Safety net:
   if (name.empty() || name == "[none]" || name == "[empty]") // Some names are reserved
   {
      ENG_LOG_ERROR("Invalid params");
      return;
   }

   // Done:
   reserved->name = name;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Get node name.
 * @return node name
 */
const std::string ENG_API &Eng::Object::getName() const
{
   return reserved->name;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Get object UID.
 * @return UID
 */
uint32_t ENG_API Eng::Object::getId() const
{
   return reserved->id;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Get number of currently allocated objects. This counter shall be zero in the end.
 * @return number of allocated objects
 */
int32_t ENG_API Eng::Object::getNrOfObjects()
{
   return counter;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Set the dirty flag.
 * @param dirty dirty flag
 */
void ENG_API Eng::Object::setDirty(bool dirty) const
{
   // First time?   
   reserved->dirty = dirty;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Return true if dirty.
 * @return true if dirty
 */
bool ENG_API Eng::Object::isDirty() const
{
   return reserved->dirty;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Empty rendering method. Bad sign if you call this.
 * @param value generic value (optional)
 * @param data generic pointer to any kind of data (optional)
 * @return TF
 */
bool ENG_API Eng::Object::render(uint32_t value, void *data) const
{
   ENG_LOG_ERROR("Empty rendering method called");
   return false;
}

