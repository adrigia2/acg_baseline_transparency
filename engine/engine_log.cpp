/**
 * @file		engine_log.cpp
 * @brief	Logging facilities
 *
 * @author	Achille Peternier (achille.peternier@supsi.ch), (C) SUPSI
 */



//////////////
// #INCLUDE //
//////////////

   // Main include:
   #include "engine.h"

   // C/C++ libs:
   #include <stdarg.h>
   #include <stdio.h>   
   #include <fstream>    



/////////////////////////
// RESERVED STRUCTURES //
/////////////////////////

/**
 * @brief Log static reserved structure.
 */
struct Eng::Log::StaticReserved
{
   std::ofstream outputFile;              ///< Textual output file   
   CustomCallbackProto customCallback;    ///< Optional callback invoked after each message


   /**
    * Constructor.
    */
   StaticReserved() : customCallback{ nullptr }
   {}
};



////////////
// STATIC //
////////////   

   // Reserved data:
   Eng::Log::StaticReserved *Eng::Log::staticReserved = nullptr; // No unique_ptr, as the pointer might go out of scope *before* the atexit invocation!



///////////////////////
// BODY OF CLASS Log //
///////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Init static components of the class.
 * @return TF
 */
bool ENG_API Eng::Log::init()
{
   // Safety net:
   if (staticReserved != nullptr)
   {
      ENG_LOG_ERROR("Static class already initialized");
      return false;
   }

   // Allocate and reset:
   staticReserved = new Eng::Log::StaticReserved();

   // Add shutdown hook:
   atexit([]()
   {         
      if (Eng::Object::getNrOfObjects() != 0)
         ENG_LOG_ERROR("Memory leak detected (parity check returned %d)", Eng::Object::getNrOfObjects());
      Eng::Log::free();
   });

   staticReserved->outputFile.open(filename);
   if (!staticReserved->outputFile.is_open())
   {
      std::cout << "[!] Unable to open output log file '" << filename << "'" << std::endl;
      return false;
   }

   // Done:
   return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Release static components of the class.
 * @return TF
 */
bool ENG_API Eng::Log::free()
{
   // Safety net:
   if (staticReserved == nullptr)
      return false;

   ENG_LOG_DEBUG("[-] Logging completed");

   // Release resources:
   staticReserved->outputFile.close();
   delete staticReserved;
   staticReserved = nullptr;

   // Done:
   return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Log a message. Static components are lazy-loaded at first usage.
 * @param lvl level of log (use level enum types)
 * @param fileName name of the file invoking the log
 * @param functionName name of the function invoking the log
 * @param text message, with custom series of params
 * @warning race conditions when run concurrently
 */
bool ENG_API Eng::Log::log(level lvl, const char *fileName, const char *functionName, int32_t codeLine, const char *text, ...)
{
   // Init at first usage:  
   if (staticReserved == nullptr)
      if (Log::init())
         ENG_LOG_DEBUG("[+] Logging to file '%s' enabled", filename);
      else
         std::cout << "[!] No logging to file for this session" << std::endl;

   // Retrieve string:
   char buffer[Log::maxLength];
   va_list list;

   // Get params:
   va_start(list, text);
   vsprintf(buffer, text, list);
   va_end(list);

   // Set values according to kind:
   const uint32_t maxPrefixSize = Log::maxLength;
   char prefix[maxPrefixSize];
   bool returnMessage = true;
   switch (lvl)
   {
      /////////////////////
      case level::plain: //
         sprintf(prefix, "%s", "");
         returnMessage = true;
         break;

      ////////////////////
      case level::info: //
         sprintf(prefix, "%s ", "[*]");
         returnMessage = true;
         break;

      ///////////////////////
      case level::warning: //
         sprintf(prefix, "%s [%s] ", "[?]", functionName);
         returnMessage = true;
         break;

      /////////////////////
      case level::error: //
         sprintf(prefix, "%s [%s, %s:%d] ", "[!]", fileName, functionName, codeLine);
         returnMessage = false;
         break;

      //////////////////////
      case level::debug:  //
      case level::detail: //
         sprintf(prefix, "%s [%s:%d] ", "[D]", functionName, codeLine);
         returnMessage = true;
         break;
   }

   // Unnecessary?
   if (lvl > Eng::Log::debugLvl)
      return returnMessage;

   // To file:
   staticReserved->outputFile << prefix << buffer << std::endl;

   // To console:
   std::cout << prefix << buffer << std::endl;

   // Custom callback?
   if (staticReserved->customCallback)
      staticReserved->customCallback(buffer, lvl, nullptr);

   // Done:
   return returnMessage;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Sets an optional callback that is triggered when a message occurs.
 * @param cb custom callback (nullptr to disable)
 */
void ENG_API Eng::Log::setCustomCallback(CustomCallbackProto cb)
{
   // Init at first usage:  
   if (staticReserved == nullptr)
      if (Log::init())
         ENG_LOG_DEBUG("[+] Logging to file '%s' enabled", filename);
      else
         std::cout << "[!] No logging to file for this session" << std::endl;

   // Release resources:
   staticReserved->customCallback = cb;
}

