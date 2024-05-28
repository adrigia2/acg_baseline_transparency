/**
 * @file		engine_ssbo.cpp
 * @brief	OpenGL Shader Storage Buffer Object (VBO)
 *
 * @author	Achille Peternier (achille.peternier@supsi.ch), (C) SUPSI
 */


//////////////
// #INCLUDE //
//////////////

// Main include:
#include "engine.h"
#include "engine_ssbo.h"

// OGL:      

#include <GL/glew.h>
#include <GLFW/glfw3.h>


////////////
// STATIC //
////////////

// Special values:
Eng::Ssbo Eng::Ssbo::empty("[empty]");


/////////////////////////
// RESERVED STRUCTURES //
/////////////////////////

/**
 * @brief Ssbo reserved structure.
 */
struct Eng::Ssbo::Reserved
{
    GLuint oglId; ///< OpenGL shader ID
    uint64_t size; ///< Size in bytes


    /**
     * Constructor.
     */
    Reserved() : oglId{0}, size{0}
    {
    }
};


////////////////////////
// BODY OF CLASS Ssbo //
////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Constructor.
 */
ENG_API Eng::Ssbo::Ssbo() : reserved(std::make_unique<Eng::Ssbo::Reserved>())
{
    ENG_LOG_DEBUG("[+]");
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Constructor with name.
 * @param name node name
 */
ENG_API Eng::Ssbo::Ssbo(const std::string& name) : Eng::Object(name), reserved(std::make_unique<Eng::Ssbo::Reserved>())
{
    ENG_LOG_DEBUG("[+]");
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Move constructor.
 */
ENG_API Eng::Ssbo::Ssbo(Ssbo&& other) : Eng::Object(std::move(other)), Eng::Managed(std::move(other)),
                                        reserved(std::move(other.reserved))
{
    ENG_LOG_DEBUG("[M]");
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Destructor.
 */
ENG_API Eng::Ssbo::~Ssbo()
{
    ENG_LOG_DEBUG("[-]");
    if (reserved)
        this->free();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Return the GLuint buffer ID.
 * @return buffer ID or 0 if not valid
 */
uint32_t ENG_API Eng::Ssbo::getOglHandle() const
{
    return reserved->oglId;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Return the size in bytes of the buffer.
 * @return size in bytes
 */
uint64_t ENG_API Eng::Ssbo::getSize() const
{
    return reserved->size;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Initializes an OpenGL SSBO.
 * @return TF
 */
bool ENG_API Eng::Ssbo::init()
{
    if (this->Eng::Managed::init() == false)
        return false;

    // Free buffer if already stored:
    if (reserved->oglId)
    {
        glDeleteBuffers(1, &reserved->oglId);
        reserved->oglId = 0;
        reserved->size = 0;
    }

    // Create it:		    
    glGenBuffers(1, &reserved->oglId);

    // Done:   
    return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Releases an OpenGL SSBO.
 * @return TF
 */
bool ENG_API Eng::Ssbo::free()
{
    if (this->Eng::Managed::free() == false)
        return false;

    // Free SSBO if stored:
    if (reserved->oglId)
    {
        glDeleteBuffers(1, &reserved->oglId);
        reserved->oglId = 0;
        reserved->size = 0;
    }

    // Done:   
    return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Create buffer by allocating the required storage.
 * @param size size in bytes
 * @param data pointer to the data to copy into the buffer 
 * @return TF
 */
bool ENG_API Eng::Ssbo::create(uint64_t size, const void* data)
{
    // Release, if already used:
    if (this->isInitialized())
        this->free();

    // Init buffer:
    if (!this->isInitialized())
        this->init();

    // Fill it:		              
    const GLuint oglId = this->getOglHandle();
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, oglId);
    glBufferStorage(GL_SHADER_STORAGE_BUFFER, size, data,
                    GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);

    // Done:
    reserved->size = size;
    return true;
}

bool ENG_API Eng::Ssbo::create(uint64_t size, const void* data, const GLbitfield flags)
{
    // Release, if already used:
    if (this->isInitialized())
        this->free();

    // Init buffer:
    if (!this->isInitialized())
        this->init();

    // Fill it:		              
    const GLuint oglId = this->getOglHandle();
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, oglId);
    glBufferData(GL_SHADER_STORAGE_BUFFER, size, data,
                    flags);

    // Done:
    reserved->size = size;
    return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Maps this SSBO for direct C-sided operations. 
 * @param mapping kind of mapping (use enums)
 * @return pointer to the mapped area or nullptr on error
 */
void ENG_API* Eng::Ssbo::map(Eng::Ssbo::Mapping mapping)
{
    GLint bufMask = 0;

    // Bind buffer and map:   
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, reserved->oglId); // <-- rendering to base 0 by default!
    switch (mapping)
    {
    case Mapping::read: bufMask = GL_MAP_READ_BIT;
        break;
    case Mapping::write: bufMask = GL_MAP_WRITE_BIT;
        break;
    }
    bufMask |= GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
    return glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, reserved->size, bufMask);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Unmap this SSBO.  
 * @return TF
 */
bool ENG_API Eng::Ssbo::unmap()
{
    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

    // Done:
    return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Rendering method. 
 * @param value generic value
 * @param data generic pointer to any kind of data
 * @return TF
 */
bool ENG_API Eng::Ssbo::render(uint32_t value, void* data) const
{
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, value, reserved->oglId);

    // Done:
    return true;
}
