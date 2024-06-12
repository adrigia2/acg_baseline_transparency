// Main includes:
#include "engine_pipeline_OIT.h"
#include "engine.h"

// OGL:
#include "engine_pipeline_OIT.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "engine_acbo.h"
#include "engine_pbo.h"
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

uniform uint maxNodes;

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


 
void main()
{
    // Get the index of the next empty slot in the buffer
  uint nodeIdx = atomicCounterIncrement(nextNodeCounter);

  if( nodeIdx < maxNodes ) {

    uint prevHead = imageAtomicExchange(headPointers, ivec2(gl_FragCoord.xy), nodeIdx);

    nodes[nodeIdx].color = vec4(compute_color(), mtlOpacity);
    nodes[nodeIdx].depth = gl_FragCoord.z;
    nodes[nodeIdx].next = prevHead;
  }

   //outFragment = nodes[nodeIdx].color;      
})";

static const std::string pipeline_cs = R"(

#define MAX_FRAGMENTS 75

layout(local_size_x = 32, local_size_y = 32) in;

struct NodeType {
  vec4 color;
  float depth;
  uint next;
};

layout(binding = 1, rgba8) uniform image2D resultImage;
layout(binding = 0, std430) buffer linkedLists {
  NodeType nodes[];
};
layout(binding = 0, r32ui) uniform uimage2D headPointers;


void main() {

    ivec2 pixelCoord = ivec2(gl_GlobalInvocationID.xy);
    ivec2 size = imageSize(resultImage);

    NodeType frags[MAX_FRAGMENTS];
    int count = 0;

    uint n = imageLoad(headPointers, pixelCoord).r;

    while (n != 0xffffffff && count < MAX_FRAGMENTS) {
        frags[count] = nodes[n];
        n = frags[count].next;
        count++;
    }

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

    float value = (pixelCoord.x+pixelCoord.y)/float(size.x+size.y);
    vec4 color = vec4(value, 1.0-value, 0.0, 1.0);
    //vec4 color = vec4(1.0, 1.0, 1.0, 1.0);
    for (int i = 0; i < count; i++) {
        color = mix(color, frags[i].color, frags[i].color.a);
    }

    imageStore(resultImage, pixelCoord, color);
}


)";

struct Eng::PipelineOIT::Reserved
{
    Eng::Shader vs;
    Eng::Shader fs;
    Eng::Shader cs;

    Eng::Program program;
    Eng::Program programCS;

    Eng::Texture renderTexture;

    Eng::Acbo acbo;
    Eng::TextureStorage textureStorage;
    Eng::Ssbo ssbo;

    //Eng::Pbo pbo;

    GLuint clearBufferId;

    GLuint maxNodes = 20 * Eng::Base::dfltWindowSizeX * Eng::Base::dfltWindowSizeY;
    GLint nodeSize = 5 * sizeof(GLfloat) + sizeof(GLuint); // The size of a linked list node


    bool wireframe;

    /**
     * Constructor. 
     */
    Reserved() : wireframe{false}
    {
    }
};

ENG_API Eng::PipelineOIT::PipelineOIT() : reserved(std::make_unique<Eng::PipelineOIT::Reserved>())
{
    ENG_LOG_DETAIL("[+]");
    this->setProgram(reserved->program);
}

Eng::PipelineOIT::PipelineOIT(const std::string& name) : Eng::Pipeline(name),
                                                         reserved(std::make_unique<Eng::PipelineOIT::Reserved>())
{
    ENG_LOG_DETAIL("[+]");
    this->setProgram(reserved->program);
}


ENG_API Eng::PipelineOIT::PipelineOIT(PipelineOIT&& other) : Eng::Pipeline(std::move(other)),
                                                             reserved(std::move(other.reserved))
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

    reserved->cs.load(Eng::Shader::Type::compute, pipeline_cs);


    if (reserved->programCS.build({reserved->cs}) == false)
    {
        ENG_LOG_ERROR("Unable to build compute program");
        return false;
    }


    if (reserved->program.build({reserved->vs, reserved->fs}) == false)
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

    reserved->acbo.create();
    reserved->ssbo.create(reserved->maxNodes * reserved->nodeSize, NULL, GL_DYNAMIC_COPY);
    
    reserved->textureStorage.create(width, height, GL_R32UI);
    reserved->textureStorage.reset();

    reserved->renderTexture.create(width, height, Texture::Format::r8g8b8a8);


    this->setDirty(false);
    return true;
}

void Eng::PipelineOIT::clearBuffers()
{
    reserved->acbo.reset();
    reserved->textureStorage.reset();
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

const Eng::Texture& Eng::PipelineOIT::getRenderTexture() const
{
    return reserved->renderTexture;
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

    glDepthMask(GL_FALSE);

    // Just to update the cache:
    this->Eng::Pipeline::render(glm::mat4(1.0f), glm::mat4(1.0f), list);

    // Apply program:
    Eng::Program& program = getProgram();
    if (program == Eng::Program::empty)
    {
        ENG_LOG_ERROR("Invalid program");
        return false;
    }


    // Wireframe is on?
    if (isWireframe())
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // Multipass rendering:
    //uint32_t totNrOfLights = list.getNrOfLights();
    
    uint32_t totNrOfLights = 1;

    //for (uint32_t l = 0; l < totNrOfLights; l++)
    //{
    program.render();
    program.setMat4("projectionMat", proj);

    program.setUInt("totNrOfLights", totNrOfLights);
    program.setUInt("maxNodes", reserved->maxNodes);

    clearBuffers();
    reserved->textureStorage.render(0);
    reserved->acbo.render(0);
    reserved->ssbo.render(0);

    // Enable addictive blending from light 1 on:
    //if (l == 1)
    //{
    //    glEnable(GL_BLEND);
    //    glBlendFunc(GL_ONE, GL_ONE);
    //}

    // Render one light at time:
    const Eng::List::RenderableElem& lightRe = list.getRenderableElem(0);
    //const Eng::List::RenderableElem& lightRe = list.getRenderableElem(l);

    const Eng::Light& light = dynamic_cast<const Eng::Light&>(lightRe.reference.get());

    // Re-enable this pipeline's program:
    program.render();
    glm::mat4 lightFinalMatrix = camera * lightRe.matrix; // Light position in eye coords
    lightRe.reference.get().render(0, &lightFinalMatrix);

    lightFinalMatrix = light.getProjMatrix() * glm::inverse(lightRe.matrix) * glm::inverse(camera);
    // To convert from eye coords into light space    
    program.setMat4("lightMatrix", lightFinalMatrix);

    // Render meshes:
    list.render(camera, proj, Eng::List::Pass::transparents);

    reserved->programCS.render();

    reserved->renderTexture.bindImage(1);
    reserved->textureStorage.render(0);
    reserved->ssbo.render(0);

    reserved->programCS.compute(reserved->renderTexture.getSizeX() / 32, reserved->renderTexture.getSizeY() / 32);
    reserved->programCS.wait();
    //}


    // Disable blending, in case we used it:
    //if (list.getNrOfLights() > 1)
    //    glDisable(GL_BLEND);

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
