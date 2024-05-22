/**
 * @file		engine_pipeline.h
 * @brief	Generic rendering pipeline
 *
 * @author	Achille Peternier (achille.peternier@supsi.ch), (C) SUPSI
 */
#pragma once



/**
 * @brief Class for setting up a rendering pipeline: derive and customize at will.
 */
class ENG_API Pipeline : public Eng::Object, public Eng::Managed
{
//////////
public: //
//////////

   // Special values:
   static Pipeline empty;   

   // Const/dest:
	Pipeline();      
	Pipeline(Pipeline &&other);
   Pipeline(Pipeline const&) = delete;   
   virtual ~Pipeline();     

   // Management:
   bool setProgram(Eng::Program &program);
   Eng::Program &getProgram() const;

   // Rendering methods:   
   virtual bool render(const Eng::Camera &camera, const Eng::List &list);
   virtual bool render(const glm::mat4 &camera, const glm::mat4 &proj, const Eng::List &list);

   // Cache:
   static Pipeline &getCached();


/////////////
protected: //
/////////////

   // Cache:
   static std::reference_wrapper<Eng::Pipeline> cache;

   // Reserved:
   struct Reserved;           
   std::unique_ptr<Reserved> reserved;			

   // Const/dest:
   Pipeline(const std::string &name);
};




