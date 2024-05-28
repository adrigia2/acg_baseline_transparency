

// Main includes:
#include "engine_pipeline_OIT.h"
#include "engine.h"

// OGL:
#include "engine_pipeline_OIT.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtx/dual_quaternion.hpp>
#include <glm/gtx/dual_quaternion.hpp>
#include <glm/gtx/dual_quaternion.hpp>
#include <glm/gtx/dual_quaternion.hpp>
#include <glm/gtx/dual_quaternion.hpp>
#include <glm/gtx/dual_quaternion.hpp>
#include <glm/gtx/dual_quaternion.hpp>
#include <glm/gtx/dual_quaternion.hpp>
#include <glm/gtx/dual_quaternion.hpp>
#include <glm/gtx/dual_quaternion.hpp>
#include <glm/gtx/dual_quaternion.hpp>
#include <glm/gtx/dual_quaternion.hpp>

#include "engine_acbo.h"
#include "engine_ssbo.h"
#include "engine_texture_storage.h"


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

layout (early_fragment_tests) in;
#define MAX_FRAGMENTS 75

struct NodeType {
  vec4 color;
  float depth;
  uint next;
};

layout( binding = 0, r32ui) uniform uimage2D headPointers;
layout( binding = 0, offset = 0) uniform atomic_uint nextNodeCounter;
layout( binding = 0, std430 ) buffer linkedLists {
  NodeType nodes[];
};
uniform uint MaxNodes;

subroutine void RenderPassType();
subroutine uniform RenderPassType RenderPass;

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

vec3 compute_color()
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

   return (mtlEmission / float(totNrOfLights)) + fragColor * albedo_texel.xyz;
}

  subroutine(RenderPassType)
void pass1()
{
  // Get the index of the next empty slot in the buffer
  uint nodeIdx = atomicCounterIncrement(nextNodeCounter);

  // Is our buffer full?  If so, we don't add the fragment
  // to the list.
  if( nodeIdx < MaxNodes ) {

    // Our fragment will be the new head of the linked list, so
    // replace the value at gl_FragCoord.xy with our new node's
    // index.  We use imageAtomicExchange to make sure that this
    // is an atomic operation.  The return value is the old head
    // of the list (the previous value), which will become the
    // next element in the list once our node is inserted.
    uint prevHead = imageAtomicExchange(headPointers, ivec2(gl_FragCoord.xy), nodeIdx);

    // Here we set the color and depth of this new node to the color
    // and depth of the fragment.  The next pointer, points to the
    // previous head of the list.
    nodes[nodeIdx].color = vec4(compute_color(), 1);
    nodes[nodeIdx].depth = gl_FragCoord.z;
    nodes[nodeIdx].next = prevHead;
  }
  //FragColor = nodes[nodeIdx].color;
}




  subroutine(RenderPassType)
void pass2()
{
  NodeType frags[MAX_FRAGMENTS];

  int count = 0;

  // Get the index of the head of the list
  uint n = imageLoad(headPointers, ivec2(gl_FragCoord.xy)).r;

  // Copy the linked list for this fragment into an array
  while( n != 0xffffffff && count < MAX_FRAGMENTS) {
    frags[count] = nodes[n];
    n = frags[count].next;
    count++;
  }

  // Sort the array by depth using insertion sort (largest
  // to smallest).

  //insert sort 
  for( uint i = 1; i < count; i++ )
  {
    NodeType toInsert = frags[i];
    uint j = i;
    while( j > 0 && toInsert.depth > frags[j-1].depth ) {
      frags[j] = frags[j-1];
      j--;
    }
    frags[j] = toInsert;
  }
  

   
  // Traverse the array, and combine the colors using the alpha
  // channel.
  vec4 color = vec4(0.5, 0.5, 0.5, 1.0);
  for( int i = 0; i < count; i++ )
  {
    color = mix( color, frags[i].color, frags[i].color.a);
  }

  // Output the final color
  outFragment = color;
}




 
void main()
{
   outFragment = vec4(compute_color(), 1);      
})";

struct Eng::PipelineOIT::Reserved
{  
   Eng::Shader vs;
   Eng::Shader fs;
   Eng::Program program;
   
   Eng::Acbo acbo;
   Eng::TextureStorage textureStorage;
   Eng::Ssbo ssbo;
   
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

   // init ACBO:
   if (reserved->acbo.init() == false)
   {
      ENG_LOG_ERROR("Unable to init ACBO");
      return false;
   }

   int width = Eng::Base::dfltWindowSizeX;
   int height = Eng::Base::dfltWindowSizeY;
   
   glGenBuffers(2, buffers);
   GLuint maxNodes = 20 * width * height;
   GLint nodeSize = 5 * sizeof(GLfloat) + sizeof(GLuint); // The size of a linked list node

   // Our atomic counter
   glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, buffers[COUNTER_BUFFER]);
   glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), NULL, GL_DYNAMIC_DRAW);

   // The buffer for the head pointers, as an image texture
   glGenTextures(1, &headPtrTex);
   glBindTexture(GL_TEXTURE_2D, headPtrTex);
   glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32UI, width, height);
   glBindImageTexture(0, headPtrTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);

   // The buffer of linked lists
   glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, buffers[LINKED_LIST_BUFFER]);
   glBufferData(GL_SHADER_STORAGE_BUFFER, maxNodes * nodeSize, NULL, GL_DYNAMIC_DRAW);

   reserved->program.setInt("MaxNodes", maxNodes);

   std::vector<GLuint> headPtrClearBuf(width*height, 0xffffffff);
   glGenBuffers(1, & clearBuf);
   glBindBuffer(GL_PIXEL_UNPACK_BUFFER, clearBuf);
   glBufferData(GL_PIXEL_UNPACK_BUFFER, headPtrClearBuf.size() * sizeof(GLuint),
       &headPtrClearBuf[0], GL_STATIC_COPY);
   

   // Done: 
   this->setDirty(false);
   return true;
}

void Eng::PipelineOIT::ClearBuffers()
{

   int width = Eng::Base::dfltWindowSizeX;
   int height = Eng::Base::dfltWindowSizeY;
   
   GLuint zero = 0;
   glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, buffers[COUNTER_BUFFER] );
   glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &zero);

   glBindBuffer(GL_PIXEL_UNPACK_BUFFER, clearBuf);
   glBindTexture(GL_TEXTURE_2D, headPtrTex);
   glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RED_INTEGER,
       GL_UNSIGNED_INT, NULL);
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


      ClearBuffers();
      pass1(camera, proj, list);
      pass2();
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

void Eng::PipelineOIT::pass1(const glm::mat4& camera, const glm::mat4& proj, const Eng::List& list) {
   glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 1, &pass1Index);

   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glDepthMask( GL_FALSE );

   // draw scene
   list.render(camera, proj, Eng::List::Pass::transparents);      

   glFinish();
}

void Eng::PipelineOIT::pass2() {
   glMemoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );

   glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 1, &pass2Index);

   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

   // Draw a screen filler
   glBindVertexArray(fsQuad);
   glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
   glBindVertexArray(0);
}


bool Eng::PipelineOIT::render(const Eng::Camera& camera, const Eng::List& list)
{
   return this->render(glm::inverse(camera.getWorldMatrix()), camera.getProjMatrix(), list);
}



