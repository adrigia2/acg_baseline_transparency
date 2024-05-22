/**
 * @file		engine_log.h
 * @brief	Logging facilities
 *
 * @author	Achille Peternier (achille.peternier@supsi.ch), (C) SUPSI
 */
#pragma once



/////////////
// #DEFINE //
/////////////

   // Macros for logging (including method and lines):         
   #define __FILENAME__                 (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)                 ///< Commodity macro for getting the filename only
   #define ENG_LOG(kind, message, ...)  Eng::Log::log(kind, __FILENAME__, __FUNCTION__, __LINE__, message, ##__VA_ARGS__)  ///< More or less verbose logging command          
   #define ENG_LOG_ERROR(message, ...)  ENG_LOG(Eng::Log::level::error, message, ##__VA_ARGS__)
   #define ENG_LOG_WARN(message, ...)   ENG_LOG(Eng::Log::level::warning, message, ##__VA_ARGS__)
   #define ENG_LOG_PLAIN(message, ...)  ENG_LOG(Eng::Log::level::plain, message, ##__VA_ARGS__)            
   #define ENG_LOG_INFO(message, ...)   ENG_LOG(Eng::Log::level::info, message, ##__VA_ARGS__)
   #define ENG_LOG_DEBUG(message, ...)  ENG_LOG(Eng::Log::level::debug, message, ##__VA_ARGS__)
   #define ENG_LOG_DETAIL(message, ...) ENG_LOG(Eng::Log::level::detail, message, ##__VA_ARGS__)      



/**
 * @brief Logging facilities. Static components are lazy-loaded at first usage. Beware that it is not thread-safe yet.
 */
class ENG_API Log final
{
//////////
public: //
//////////

   // Constants:
   static constexpr uint32_t maxLength = 65536;                   ///< Maximum size of a log message
   static constexpr const char filename[] = "engine.log";         ///< Output logging filename


   /**
    * @brief Logging levels.
    */
   enum class level : uint32_t
   {
      none,          ///< No logging, used for levels
      error,         ///< Error message
      warning,       ///< Warning message
      plain,         ///< Clean simple output message
      info,          ///< Hint or additional information (slightly verbose)
      debug,         ///< Debugging information (very verbose)
      detail,        ///< Debugging information (extremely verbose)
      last,          ///< Terminator
   };

#ifdef _DEBUG
   static constexpr const level debugLvl = level::debug;    ///< Logging message level
#else
   static constexpr const level debugLvl = level::info;    ///< Logging message level
#endif

   // Const/dest:
   Log() = delete;
   ~Log() = delete;

   // Log:
   static bool log(level lvl, const char *filename, const char *functionName, int32_t codeLine, const char *text, ...);

   // Parser proto:
   typedef bool(*CustomCallbackProto)(char *msg, level lvl, void *data);

   // Get/set:
   static void setCustomCallback(CustomCallbackProto cb);


///////////
private: //
///////////

   // Reserved:
   struct StaticReserved;
   static StaticReserved *staticReserved;

   // Init/free:
   static bool init();
   static bool free();
};



