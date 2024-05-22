/**
 * @file		engine_pipeline.cpp
 * @brief	Generic rendering pipeline
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
   Eng::Pipeline Eng::Pipeline::empty("[empty]");   

   // Cache:
   std::reference_wrapper<Eng::Pipeline> Eng::Pipeline::cache = Eng::Pipeline::empty;



/////////////////////////
// RESERVED STRUCTURES //
/////////////////////////

/**
 * @brief Pipeline reserved structure.
 */
struct Eng::Pipeline::Reserved
{     
   std::reference_wrapper<Eng::Program> program;  ///< Program of the pipeline


   /**
    * Constructor. 
    */
   Reserved() : program{ Eng::Program::empty }
   {}
};



////////////////////////////
// BODY OF CLASS Pipeline //
////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Constructor.
 */
ENG_API Eng::Pipeline::Pipeline() : reserved(std::make_unique<Eng::Pipeline::Reserved>())
{	
   ENG_LOG_DETAIL("[+]");   
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Constructor with name.
 * @param name node name
 */
ENG_API Eng::Pipeline::Pipeline(const std::string &name) : Eng::Object(name), reserved(std::make_unique<Eng::Pipeline::Reserved>())
{	   
   ENG_LOG_DETAIL("[+]");   
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Move constructor. 
 */
ENG_API Eng::Pipeline::Pipeline(Pipeline &&other) : Eng::Object(std::move(other)), Eng::Managed(std::move(other)), reserved(std::move(other.reserved))
{  
   ENG_LOG_DETAIL("[M]");
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Destructor.
 */
ENG_API Eng::Pipeline::~Pipeline()
{	
   ENG_LOG_DETAIL("[-]");
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Set pipeline program. 
 * @param program pipeline program 
 * @return TF
 */
bool ENG_API Eng::Pipeline::setProgram(Eng::Program &program)
{	
   reserved->program = program;

   // Done:
   return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Get pipeline program.  
 * @return pipeline program
 */
Eng::Program ENG_API &Eng::Pipeline::getProgram() const
{	
   return reserved->program;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Get the last rendered pipeline.
 * @return last rendered pipeline
 */
Eng::Pipeline ENG_API &Eng::Pipeline::getCached()
{
   return cache;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Main rendering method for the pipeline.
 * @param camera camera matrix
 * @param proj projection matrix
 * @param list list of renderables
 * @return TF
 */
bool ENG_API Eng::Pipeline::render(const glm::mat4 &camera, const glm::mat4 &proj, const Eng::List &list)
{
   // Just update cache:
   Eng::Pipeline::cache = const_cast<Eng::Pipeline &>(*this);   
   return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Shortcut for using a camera instead of the explicit matrices.
 * @param camera camera to use
 * @param list list of renderables
 * @return TF
 */
bool ENG_API Eng::Pipeline::render(const Eng::Camera &camera, const Eng::List &list)
{	
   return this->render(glm::inverse(camera.getWorldMatrix()), camera.getProjMatrix(), list);
}
