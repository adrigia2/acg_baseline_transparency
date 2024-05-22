/**
 * @file		engine_managed.cpp
 * @brief	Basic manager for objects with particular life cycle
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

   // Keep track of created instances:
   std::list<std::reference_wrapper<Eng::Managed>> allManaged;



/////////////////////////
// RESERVED STRUCTURES //
/////////////////////////

/**
 * @brief Managed reserved structure.
 */
struct Eng::Managed::Reserved
{  
   bool initialized;    ///< True when the object is allocated on the device 


   /**
    * Constructor.
    */
   Reserved() : initialized{ false }
   {}
};



///////////////////////////
// BODY OF CLASS Managed //
///////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Constructor.
 */
ENG_API Eng::Managed::Managed() : reserved(std::make_unique<Eng::Managed::Reserved>())
{
   ENG_LOG_DETAIL("[+]");
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Move constructor.
 */
ENG_API Eng::Managed::Managed(Managed &&other) : reserved(std::move(other.reserved))
{
   ENG_LOG_DETAIL("[M]");

   // Update the reference:
   for (auto it = allManaged.begin(); it != allManaged.end(); ++it)
      if (it->get().reserved == other.reserved)
      {
         *it = *this; 
         break;
      }
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Destructor.
 */
ENG_API Eng::Managed::~Managed()
{
   ENG_LOG_DETAIL("[-]");

   if (!allManaged.empty())
      for (auto it = allManaged.begin(); it != allManaged.end(); ++it)
         if (it->get().reserved == this->reserved)
         {  
            allManaged.erase(it); // Already done in free
            break;
         }
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Initializes the managed object. 
 * @return TF
 */
bool ENG_API Eng::Managed::init()
{
   // Safety net:
   if (reserved && reserved->initialized)
   {
      ENG_LOG_ERROR("Object already initialized");
      return false;
   }

   // Add to the list:
   allManaged.push_back(*this);

   // Done:
   reserved->initialized = true;
   return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Releases the managed object. 
 * @return TF
 */
bool ENG_API Eng::Managed::free()
{
   // Safety net:
   if (!reserved) // Because of the move constructor
      return true; 
   if (!reserved->initialized)
   {
      //OV_LOG_WARN("Object not initialized");
      return true;
   }
   
   // Remove from list:
   if (!allManaged.empty())
      for (auto it = allManaged.begin(); it != allManaged.end(); ++it)
         if (it->get().reserved == this->reserved)
         {
            allManaged.erase(it);
            break;
         }   

   // Done:
   reserved->initialized = false;
   return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Passes the list and forces resource deallocation. 
 * @return TF
 */
bool ENG_API Eng::Managed::forceRelease()
{
   ENG_LOG_DEBUG("Forced release of managed objects...");

   uint64_t total = 0, initialized = 0;
   if (!allManaged.empty())
   {
      auto it = allManaged.begin();
      while (it != allManaged.end()) 
      {
         total++;
         bool isInitialized = it->get().isInitialized();
         if (isInitialized)
         {
            initialized++;
            auto _it = it;            
            it++;            
            _it->get().free();
         }
         else
            ++it;         
      }
   }

   // Done:
   ENG_LOG_DEBUG("%llu managed object(s) released out of %llu", initialized, total);
   return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Passes the list and forces resource deallocation.
 * @return TF
 */
void ENG_API Eng::Managed::dumpReport()
{
   uint64_t total = 0, initialized = 0;
   for (auto &m : allManaged)
   {
      total++;
      if (m.get().isInitialized())
         initialized++;           
   }

   // Done:
   ENG_LOG_PLAIN("%llu managed object(s), %llu initialized", total, initialized); 
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Returns true when the object is initialized. 
 * @return TF
 */
bool ENG_API Eng::Managed::isInitialized() const
{   
   return reserved->initialized;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Sets the status of the initialized flag.
 * @param initializedFlag initialized flag status
 */
void ENG_API Eng::Managed::setInitialized(bool initializedFlag)
{
   reserved->initialized = initializedFlag;
}
