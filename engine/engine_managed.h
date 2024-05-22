/**
 * @file		engine_managed.h
 * @brief	Basic manager for objects with particular life cycle
 *
 * @author	Achille Peternier (achille.peternier@supsi.ch), (C) SUPSI
 */
#pragma once



/**
 * @brief Class for managing objects with a particular life cycle.
 */
class ENG_API Managed
{
//////////
public: //
//////////

    // Const/dest:
   Managed();
   Managed(Managed &&other);
   Managed(Managed const &) = delete;
   virtual ~Managed();

   // Operators:
   void operator=(Managed const &) = delete;  

   // Management methods: 
   virtual bool init();
   virtual bool free();   
   static bool forceRelease();
   static void dumpReport();

   // Get/set:
   bool isInitialized() const;   


/////////////
protected: //
/////////////

   // Get/set:
   void setInitialized(bool initializedFlag);


///////////
private: //
///////////

   // Reserved:   
   struct Reserved;
   std::unique_ptr<Reserved> reserved;   
};




