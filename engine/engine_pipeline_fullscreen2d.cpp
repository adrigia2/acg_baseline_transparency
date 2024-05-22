/**
 * @file		engine_pipeline_fullscreen2d.cpp 
 * @brief	A pipeline for rendering a texture to the fullscreen in 2D
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



/////////////
// SHADERS //
/////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Default pipeline vertex shader.
 */
static const std::string pipeline_vs = R"(

// Out:
out vec2 texCoord;


void main()
{   
   float x = -1.0f + float((gl_VertexID & 1) << 2);
   float y = -1.0f + float((gl_VertexID & 2) << 1);
   
   texCoord.x = (x + 1.0f) * 0.5f;
   texCoord.y = (y + 1.0f) * 0.5f;
   
   gl_Position = vec4(x, y, 1.0f, 1.0f);
})";


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Default pipeline fragment shader.
 */
static const std::string pipeline_fs = R"(
   
// Uniform:
#ifdef ENG_BINDLESS_SUPPORTED
   layout (bindless_sampler) uniform sampler2D texture0;
#else
   layout (binding = 0) uniform sampler2D texture0;    
#endif

// In:   
in vec2 texCoord;
   
// Out:
out vec4 outFragment;


void main()
{
   outFragment = texture(texture0, texCoord);   
})";



/////////////////////////
// RESERVED STRUCTURES //
/////////////////////////

/**
 * @brief PipelineFullscreen2D reserved structure.
 */
struct Eng::PipelineFullscreen2D::Reserved
{  
   Eng::Shader vs;
   Eng::Shader fs;
   Eng::Program program;      
   Eng::Vao vao;  ///< Dummy VAO, always required by context profiles


   /**
    * Constructor. 
    */
   Reserved()
   {}
};



////////////////////////////////////////
// BODY OF CLASS PipelineFullscreen2D //
////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Constructor.
 */
ENG_API Eng::PipelineFullscreen2D::PipelineFullscreen2D() : reserved(std::make_unique<Eng::PipelineFullscreen2D::Reserved>())
{	
   ENG_LOG_DETAIL("[+]");      
   this->setProgram(reserved->program);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Constructor with name.
 * @param name node name
 */
ENG_API Eng::PipelineFullscreen2D::PipelineFullscreen2D(const std::string &name) : Eng::Pipeline(name), reserved(std::make_unique<Eng::PipelineFullscreen2D::Reserved>())
{	   
   ENG_LOG_DETAIL("[+]");   
   this->setProgram(reserved->program);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Move constructor. 
 */
ENG_API Eng::PipelineFullscreen2D::PipelineFullscreen2D(PipelineFullscreen2D &&other) : Eng::Pipeline(std::move(other)), reserved(std::move(other.reserved))
{  
   ENG_LOG_DETAIL("[M]");
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Destructor.
 */
ENG_API Eng::PipelineFullscreen2D::~PipelineFullscreen2D()
{	
   ENG_LOG_DETAIL("[-]");
   if (this->isInitialized())
      free();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Initializes this pipeline. 
 * @return TF
 */
bool ENG_API Eng::PipelineFullscreen2D::init()
{
   // Already initialized?
   if (this->Eng::Managed::init() == false)
      return false;
   if (!this->isDirty())
      return false;

   // Build:
   reserved->vs.load(Eng::Shader::Type::vertex, pipeline_vs);
   reserved->fs.load(Eng::Shader::Type::fragment, pipeline_fs);   
   if (reserved->program.build({ reserved->vs, reserved->fs }) == false)
   {
      ENG_LOG_ERROR("Unable to build fullscreen2D program");
      return false;
   }
   this->setProgram(reserved->program);   

   // Init dummy VAO:
   if (reserved->vao.init() == false)
   {
      ENG_LOG_ERROR("Unable to init VAO for fullscreen2D");
      return false;
   }

   // Done: 
   this->setDirty(false);
   return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Releases this pipeline.
 * @return TF
 */
bool ENG_API Eng::PipelineFullscreen2D::free()
{
   if (this->Eng::Managed::free() == false)
      return false;

   // Done:   
   return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Main rendering method for the pipeline.  
 * @param camera view camera
 * @param list list of renderables
 * @return TF
 */
bool ENG_API Eng::PipelineFullscreen2D::render(const Eng::Texture &texture, const Eng::List &list)
{	
   // Safety net:
   if (texture == Eng::Texture::empty || list == Eng::List::empty)
   {
      ENG_LOG_ERROR("Invalid params");
      return false;
   }

   // Just to update the cache
   this->Eng::Pipeline::render(glm::mat4(1.0f), glm::mat4(1.0f), list); 

   // Lazy-loading:
   if (this->isDirty())
      if (!this->init())
      {
         ENG_LOG_ERROR("Unable to render (initialization failed)");
         return false;
      }

   // Apply program:
   Eng::Program &program = getProgram();
   if (program == Eng::Program::empty)
   {
      ENG_LOG_ERROR("Invalid program");
      return false;
   }   
   program.render();     
   texture.render(0);
   
   Eng::Base &eng = Eng::Base::getInstance();
   Eng::Fbo::reset(eng.getWindowSize().x, eng.getWindowSize().y);   

   // Smart trick:   
   reserved->vao.render();
   glDrawArrays(GL_TRIANGLES, 0, 3);
  
   // Done:   
   return true;
}
