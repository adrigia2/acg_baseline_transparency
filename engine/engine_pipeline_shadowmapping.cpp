/**
 * @file		engine_pipeline_shadowmapping.cpp 
 * @brief	A pipeline for generating planar shadow maps
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
 
// Per-vertex data from VBOs:
layout(location = 0) in vec3 a_vertex;
layout(location = 1) in vec4 a_normal;
layout(location = 2) in vec2 a_uv;
layout(location = 3) in vec4 a_tangent;

// Uniforms:
uniform mat4 modelviewMat;
uniform mat4 projectionMat;

void main()
{   
   gl_Position = projectionMat *  modelviewMat * vec4(a_vertex, 1.0f);
})";


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Default pipeline fragment shader.
 */
static const std::string pipeline_fs = R"(

void main()
{
})";



/////////////////////////
// RESERVED STRUCTURES //
/////////////////////////

/**
 * @brief PipelineShadowMapping reserved structure.
 */
struct Eng::PipelineShadowMapping::Reserved
{  
   Eng::Shader vs;
   Eng::Shader fs;
   Eng::Program program;
   Eng::Texture depthMap;
   Eng::Fbo fbo;


   /**
    * Constructor. 
    */
   Reserved()
   {}
};



/////////////////////////////////////////
// BODY OF CLASS PipelineShadowMapping //
/////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Constructor.
 */
ENG_API Eng::PipelineShadowMapping::PipelineShadowMapping() : reserved(std::make_unique<Eng::PipelineShadowMapping::Reserved>())
{	
   ENG_LOG_DETAIL("[+]");      
   this->setProgram(reserved->program);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Constructor with name.
 * @param name node name
 */
ENG_API Eng::PipelineShadowMapping::PipelineShadowMapping(const std::string &name) : Eng::Pipeline(name), reserved(std::make_unique<Eng::PipelineShadowMapping::Reserved>())
{	   
   ENG_LOG_DETAIL("[+]");   
   this->setProgram(reserved->program);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Move constructor. 
 */
ENG_API Eng::PipelineShadowMapping::PipelineShadowMapping(PipelineShadowMapping &&other) : Eng::Pipeline(std::move(other)), reserved(std::move(other.reserved))
{  
   ENG_LOG_DETAIL("[M]");
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Destructor.
 */
ENG_API Eng::PipelineShadowMapping::~PipelineShadowMapping()
{	
   ENG_LOG_DETAIL("[-]");
   if (this->isInitialized())
      free();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Gets shadow map texture reference.
 * @return shadow map texture reference
 */
const Eng::Texture ENG_API &Eng::PipelineShadowMapping::getShadowMap() const
{	
   return reserved->depthMap;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Initializes this pipeline. 
 * @return TF
 */
bool ENG_API Eng::PipelineShadowMapping::init()
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
      ENG_LOG_ERROR("Unable to build shadow mapping program");
      return false;
   }
   this->setProgram(reserved->program);

   // Depth map:
   if (reserved->depthMap.create(depthTextureSize, depthTextureSize, Eng::Texture::Format::depth) == false)
   {
      ENG_LOG_ERROR("Unable to init depth map");
      return false;
   }

   // Depth FBO:
   reserved->fbo.attachTexture(reserved->depthMap);
   if (reserved->fbo.validate() == false)
   {
      ENG_LOG_ERROR("Unable to init depth FBO");
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
bool ENG_API Eng::PipelineShadowMapping::free()
{
   if (this->Eng::Managed::free() == false)
      return false;

   // Done:   
   return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Main rendering method for the pipeline.
 * @param camera camera matrix
 * @param proj projection matrix
 * @param list list of renderables
 * @return TF
 */
bool ENG_API Eng::PipelineShadowMapping::render(const glm::mat4 &camera, const glm::mat4 &proj, const Eng::List &list)
{	
   // Safety net:
   if (list == Eng::List::empty)
   {
      ENG_LOG_ERROR("Invalid params");
      return false;
   }   
   
   // Lazy-loading:
   if (this->isDirty())
      if (!this->init())
      {
         ENG_LOG_ERROR("Unable to render (initialization failed)");
         return false;
      }

   // Just to update the cache
   this->Eng::Pipeline::render(glm::mat4(1.0f), glm::mat4(1.0f), list);

   // Apply program:
   Eng::Program &program = getProgram();
   if (program == Eng::Program::empty)
   {
      ENG_LOG_ERROR("Invalid program");
      return false;
   }   
   program.render();    
   program.setMat4("projectionMat", proj);
   
   // Bind FBO and change OpenGL settings:
   reserved->fbo.render();
   glClear(GL_DEPTH_BUFFER_BIT);
   glColorMask(0, 0, 0, 0); // Disable color writes
   glEnable(GL_CULL_FACE);
   glCullFace(GL_FRONT);

   // Render meshes:   
   list.render(camera, proj, Eng::List::Pass::meshes);         

   // Redo OpenGL settings:
   glCullFace(GL_BACK);
   glDisable(GL_CULL_FACE);
   glColorMask(1, 1, 1, 1);
   
   Eng::Base &eng = Eng::Base::getInstance();
   Eng::Fbo::reset(eng.getWindowSize().x, eng.getWindowSize().y);   
  
   // Done:   
   return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Shortcut for using a camera instead of the explicit matrices.
 * @param camera camera to use
 * @param list list of renderables
 * @return TF
 */
bool ENG_API Eng::PipelineShadowMapping::render(const Eng::Camera &camera, const Eng::List &list)
{
   return this->render(glm::inverse(camera.getWorldMatrix()), camera.getProjMatrix(), list);
}