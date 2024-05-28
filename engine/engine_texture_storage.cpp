/**
 * @file		engine_texture.cpp
 * @brief	OpenGL texture
 *
 * @author	Achille Peternier (achille.peternier@supsi.ch), (C) SUPSI
 */


//////////////
// #INCLUDE //
//////////////

// Main include:
#include "engine.h"
#include "engine_texture_storage.h"

// OGL:      
#include <GL/glew.h>
#include <GLFW/glfw3.h>


/////////////////////////
// RESERVED STRUCTURES //
/////////////////////////

/**
 * @brief Texture reserved structure.
 */
struct Eng::TextureStorage::Reserved
{
    glm::u32vec3 size;

    GLuint oglId; ///< OpenGL texture ID   


    /**
     * Constructor. 
     */
    Reserved() : oglId{0}
    {
    }
};


///////////////////////////
// BODY OF CLASS Texture //
///////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Constructor.
 */
ENG_API Eng::TextureStorage::TextureStorage() : reserved(std::make_unique<Eng::TextureStorage::Reserved>())
{
    ENG_LOG_DETAIL("[+]");
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Constructor with name.
 * @param name node name
 */
ENG_API Eng::TextureStorage::TextureStorage(const std::string& name) : Eng::Object(name),
                                                                       reserved(
                                                                           std::make_unique<
                                                                               Eng::TextureStorage::Reserved>())
{
    ENG_LOG_DETAIL("[+]");
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Move constructor. 
 */
ENG_API Eng::TextureStorage::TextureStorage(TextureStorage&& other) : Eng::Object(std::move(other)),
                                                                      Eng::Managed(std::move(other)),
                                                                      reserved(std::move(other.reserved))
{
    ENG_LOG_DETAIL("[M]");
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Destructor.
 */
ENG_API Eng::TextureStorage::~TextureStorage()
{
    ENG_LOG_DETAIL("[-]");
    if (reserved) // Because of the move constructor   
        this->free();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Get texture size X.
 * @return texture size X
 */
uint32_t ENG_API Eng::TextureStorage::getSizeX() const
{
    return reserved->size.x;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Set texture size X.
 * @param sizeX texture width
 */
void ENG_API Eng::TextureStorage::setSizeX(uint32_t sizeX)
{
    reserved->size.x = sizeX;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Get texture size Y.
 * @return texture size Y
 */
uint32_t ENG_API Eng::TextureStorage::getSizeY() const
{
    return reserved->size.y;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Set texture size Y.
 * @param sizeY texture height
 */
void ENG_API Eng::TextureStorage::setSizeY(uint32_t sizeY)
{
    reserved->size.y = sizeY;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Return the GLuint texture ID. 
 * @return texture ID or 0 if not valid
 */
uint32_t ENG_API Eng::TextureStorage::getOglHandle() const
{
    return reserved->oglId;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Create an OpenGL instance of the texture. 
 * @return TF
 */
bool ENG_API Eng::TextureStorage::init()
{
    if (this->Eng::Managed::init() == false)
        return false;

    if (reserved->oglId)
    {
        glDeleteTextures(1, &reserved->oglId);
        reserved->oglId = 0;
    }

    // Create it:		    
    glGenTextures(1, &reserved->oglId);

    // Done:   
    return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Makes the texture resident (can't be modified anymore).
 * @return TF
 */
bool ENG_API Eng::TextureStorage::makeResident() const
{
    /*
    // Sanity check:
    if (reserved->oglBindlessHandle)
    {
        ENG_LOG_ERROR("Texture already resident");
        return false;
    }

    // Bindless:   
    reserved->oglBindlessHandle = glGetTextureHandleARB(reserved->oglId);
    glMakeTextureHandleResidentARB(reserved->oglBindlessHandle);

    */
    // Done:   
    return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Destroy an OpenGL instance. 
 * @return TF
 */
bool ENG_API Eng::TextureStorage::free()
{
    if (this->Eng::Managed::free() == false)
        return false;

    if (reserved->oglId)
    {
        glDeleteTextures(1, &reserved->oglId);
        reserved->oglId = 0;
    }

    // Done:   
    return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	 
/** 
 * Allocate memory and initialize an empty texture. 
 * @param sizeX texture width
 * @param sizeY texture height  
 * @param format pixel layout
 * @return TF
 */
bool ENG_API Eng::TextureStorage::create(uint32_t sizeX, uint32_t sizeY)
{
    if (sizeX == 0 || sizeY == 0)
    {
        ENG_LOG_ERROR("Invalid size");
        return false;
    }

    init();

    const GLuint oglId = reserved->oglId;
    glBindTexture(GL_TEXTURE_2D, oglId);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32UI, sizeX, sizeY);

    // done
    setSizeX(sizeX);
    setSizeY(sizeY);
    return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Empty rendering method. Bad sign if you read this. 
 * @param value generic value
 * @param data generic pointer to any kind of data
 * @return TF
 */
bool ENG_API Eng::TextureStorage::render(uint32_t value, void* data) const
{
    Eng::Program& program = Eng::Program::getCached();

    glBindTextures(value, 1, &reserved->oglId);
    glBindImageTexture(0, reserved->oglId, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);

    // Done:
    return true;
}
