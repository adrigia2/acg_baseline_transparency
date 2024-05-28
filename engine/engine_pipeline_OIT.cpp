

// Main includes:
#include "engine_pipeline_OIT.h"
#include "engine.h"

// OGL:
#include "engine_pipeline_OIT.h"

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
uniform mat3 normalMat;
uniform mat4 lightMatrix;

// Varying:
out vec4 fragPosition;
out vec4 fragPositionLightSpace;
out vec3 normal;
out vec2 uv;

void main()
{
   normal = normalMat * a_normal.xyz;
   uv = a_uv;

   fragPosition = modelviewMat * vec4(a_vertex, 1.0f);
   fragPositionLightSpace = lightMatrix * fragPosition;
   gl_Position = projectionMat * fragPosition;
})";


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Default pipeline fragment shader.
 */
static const std::string pipeline_fs = R"(

// Uniform:
#ifdef ENG_BINDLESS_SUPPORTED
   layout (bindless_sampler) uniform sampler2D texture0; // Albedo
   layout (bindless_sampler) uniform sampler2D texture1; // Normal
   layout (bindless_sampler) uniform sampler2D texture2; // Roughness
   layout (bindless_sampler) uniform sampler2D texture3; // Metalness
#else
   layout (binding = 0) uniform sampler2D texture0; // Albedo
   layout (binding = 1) uniform sampler2D texture1; // Normal
   layout (binding = 2) uniform sampler2D texture2; // Roughness
   layout (binding = 3) uniform sampler2D texture3; // Metalness
#endif

// Uniform (material):
uniform vec3 mtlEmission;
uniform vec3 mtlAlbedo;
uniform float mtlOpacity;
uniform float mtlRoughness;
uniform float mtlMetalness;

// Uniform (light):
uniform uint totNrOfLights;
uniform vec3 lightColor;
uniform vec3 lightAmbient;
uniform vec3 lightPosition;

// Varying:
in vec4 fragPosition;
in vec4 fragPositionLightSpace;
in vec3 normal;
in vec2 uv;

struct NodeType {
  vec4 color;
  float depth;
  uint next;
};
 
// Output to the framebuffer:
out vec4 outFragment;

//////////
// MAIN //
//////////
 
void main()
{
   // Texture lookup:
   vec4 albedo_texel = texture(texture0, uv);
   vec4 normal_texel = texture(texture1, uv);
   vec4 roughness_texel = mtlRoughness * texture(texture2, uv);
   vec4 metalness_texel = mtlMetalness * texture(texture3, uv);
   float justUseIt = albedo_texel.r + normal_texel.r + roughness_texel.r + metalness_texel.r;

   // Material props:
   justUseIt += mtlEmission.r + mtlAlbedo.r + mtlOpacity + mtlRoughness + mtlMetalness;

   vec3 fragColor = lightAmbient; 
   
   vec3 N = normalize(normal);   
   vec3 V = normalize(-fragPosition.xyz);   
   vec3 L = normalize(lightPosition - fragPosition.xyz);      

   // Light only front faces:
   if (dot(N, V) > 0.0f)
   {
      // Diffuse term:   
      float nDotL = max(0.0f, dot(N, L));      
      fragColor += roughness_texel.r * nDotL * lightColor;
      
      // Specular term:     
      vec3 H = normalize(L + V);                     
      float nDotH = max(0.0f, dot(N, H));         
      fragColor += (1.0f - roughness_texel.r) * pow(nDotH, 70.0f) * lightColor;         
   }
   
   outFragment = vec4((mtlEmission / float(totNrOfLights)) + fragColor * albedo_texel.xyz, justUseIt);      
})";

struct Eng::PipelineOIT::Reserved
{  
   Eng::Shader vs;
   Eng::Shader fs;
   Eng::Program program;
   
   bool wireframe;

   /**
    * Constructor. 
    */
   Reserved() : wireframe{ false }
   {}
};

ENG_API Eng::PipelineOIT::PipelineOIT() : reserved(std::make_unique<Eng::PipelineOIT::Reserved>())
{
   ENG_LOG_DETAIL("[+]");
   this->setProgram(reserved->program);
}

Eng::PipelineOIT::PipelineOIT(const std::string& name) : Eng::Pipeline(name), reserved(std::make_unique<Eng::PipelineOIT::Reserved>())
{
   ENG_LOG_DETAIL("[+]");
   this->setProgram(reserved->program);
}


ENG_API Eng::PipelineOIT::PipelineOIT(PipelineOIT&& other) : Eng::Pipeline(std::move(other)), reserved(std::move(other.reserved))
{
   ENG_LOG_DETAIL("[M]");
}

ENG_API Eng::PipelineOIT::~PipelineOIT()
{
   ENG_LOG_DETAIL("[-]");
   if (this->isInitialized())
      free();
}

bool Eng::PipelineOIT::init()
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
      ENG_LOG_ERROR("Unable to build default program");
      return false;
   }
   this->setProgram(reserved->program);

   // Done: 
   this->setDirty(false);
   return true;
}


bool Eng::PipelineOIT::free()
{
   if (this->Eng::Managed::free() == false)
      return false;

   // Done:   
   return true;
}

bool Eng::PipelineOIT::isWireframe() const
{
   return reserved->wireframe;
}

ENG_API void Eng::PipelineOIT::setWireframe(bool flag)
{
   reserved->wireframe = flag;
}




bool Eng::PipelineOIT::render(const glm::mat4& camera, const glm::mat4& proj, const Eng::List& list)
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

   // Just to update the cache:
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
   
   // Wireframe is on?
   if (isWireframe())
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);      

   // Multipass rendering:
   uint32_t totNrOfLights = list.getNrOfLights();
   program.setUInt("totNrOfLights", totNrOfLights);

   for (uint32_t l = 0; l < totNrOfLights; l++)
   {
      // Enable addictive blending from light 1 on:
      if (l == 1)      
      {
         glEnable(GL_BLEND);
         glBlendFunc(GL_ONE, GL_ONE);         
      }
      
      // Render one light at time:
      const Eng::List::RenderableElem &lightRe = list.getRenderableElem(l);     
      const Eng::Light &light = dynamic_cast<const Eng::Light &>(lightRe.reference.get());

      // Re-enable this pipeline's program:
      program.render();   
      glm::mat4 lightFinalMatrix = camera * lightRe.matrix; // Light position in eye coords
      lightRe.reference.get().render(0, &lightFinalMatrix);

      lightFinalMatrix = light.getProjMatrix() * glm::inverse(lightRe.matrix) * glm::inverse(camera); // To convert from eye coords into light space    
      program.setMat4("lightMatrix", lightFinalMatrix);
      
      // Render meshes:
      list.render(camera, proj, Eng::List::Pass::meshes);
      list.render(camera, proj, Eng::List::Pass::transparents);      
   }

   // Disable blending, in case we used it:
   if (list.getNrOfLights() > 1)         
      glDisable(GL_BLEND);            

   // Wireframe is on?
   if (isWireframe())
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

   // Done:   
   return true;
}


bool Eng::PipelineOIT::render(const Eng::Camera& camera, const Eng::List& list)
{
   return this->render(glm::inverse(camera.getWorldMatrix()), camera.getProjMatrix(), list);
}



