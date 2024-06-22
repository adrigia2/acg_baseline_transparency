/**
 * @file		engine_fbo.cpp
 * @brief	OpenGL framebuffer object
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


////////////
// STATIC //
////////////

// Special values:
Eng::Fbo Eng::Fbo::empty("[empty]");


/////////////////////////
// RESERVED STRUCTURES //
/////////////////////////

/**
 * @brief Fbo reserved structure.
 */
struct Eng::Fbo::Reserved
{
    GLuint oglId; ///< OpenGL framebuffer ID   
    std::vector<Attachment> attachment; ///< Array of attachments
    std::vector<GLenum> mrt; ///< List of multiple rendering target


    /**
     * Constructor. 
     */
    Reserved() : oglId{0}
    {
    }
};


////////////////////////////////////
// BODY OF STRUCT Fbo::Attachment //
////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Constructor.
 */
ENG_API Eng::Fbo::Attachment::Attachment() : type{Eng::Fbo::Attachment::Type::none}, size{0, 0}, data{0}
{
}


///////////////////////
// BODY OF CLASS Fbo //
///////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Constructor.
 */
ENG_API Eng::Fbo::Fbo() : reserved(std::make_unique<Eng::Fbo::Reserved>())
{
    ENG_LOG_DETAIL("[+]");
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Constructor with name.
 * @param name node name
 */
ENG_API Eng::Fbo::Fbo(const std::string& name) : Eng::Object(name), reserved(std::make_unique<Eng::Fbo::Reserved>())
{
    ENG_LOG_DETAIL("[+]");
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Move constructor. 
 */
ENG_API Eng::Fbo::Fbo(Fbo&& other) : Eng::Object(std::move(other)), Eng::Managed(std::move(other)),
                                     reserved(std::move(other.reserved))
{
    ENG_LOG_DETAIL("[M]");
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Destructor.
 */
ENG_API Eng::Fbo::~Fbo()
{
    ENG_LOG_DETAIL("[-]");
    if (reserved)
        this->free();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Create an OpenGL instance. 
 * @return TF
 */
bool ENG_API Eng::Fbo::init()
{
    if (this->Eng::Managed::init() == false)
        return false;

    // Free texture if already stored:
    if (reserved->oglId)
    {
        glDeleteFramebuffers(1, &reserved->oglId);
        reserved->oglId = 0;
    }

    // Create it:		    
    glGenFramebuffers(1, &reserved->oglId);

    // Done:   
    return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Destroy an OpenGL instance. 
 * @return TF
 */
bool ENG_API Eng::Fbo::free()
{
    if (this->Eng::Managed::free() == false)
        return false;

    // Delete and remove render buffers:
    for (uint32_t c = 0; c < getNrOfAttachments(); c++)
    {
        Eng::Fbo::Attachment& att = reserved->attachment.at(c);
        switch (att.type)
        {
        /////////////////////////////////////////////////         
        case Eng::Fbo::Attachment::Type::depth_buffer: //         
            GLuint oglId = static_cast<GLuint>(att.data);
            glDeleteRenderbuffers(1, &oglId);
            break;
        }

        reserved->attachment.erase(reserved->attachment.begin() + c);
    }

    // Free framebuffer if used:
    if (reserved->oglId)
    {
        glDeleteFramebuffers(1, &reserved->oglId);
        reserved->oglId = 0;
    }

    // Done:   
    return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Get number of attachments.  
 * @return number of attachments
 */
uint32_t ENG_API Eng::Fbo::getNrOfAttachments() const
{
    return static_cast<uint32_t>(reserved->attachment.size());
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Get framebuffer width.  
 * @return width
 */
uint32_t ENG_API Eng::Fbo::getSizeX() const
{
    if (reserved->attachment.size() == 0)
    {
        ENG_LOG_ERROR("Empty FBO");
        return 0;
    }
    return reserved->attachment[0].size.x;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Get framebuffer height.  
 * @return height
 */
uint32_t ENG_API Eng::Fbo::getSizeY() const
{
    if (reserved->attachment.size() == 0)
    {
        ENG_LOG_ERROR("Empty FBO");
        return 0;
    }
    return reserved->attachment[0].size.y;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Add a texture in the next slot of the framebuffer. 
 * @param texture texture
 * @return TF
 */
bool ENG_API Eng::Fbo::attachTexture(const Eng::Texture& texture, uint32_t level, uint32_t side)
{
    // Safety net:
    if (texture == Eng::Texture::empty)
    {
        ENG_LOG_ERROR("Invalid params");
        return false;
    }

    // Lazy-load on first attachment:
    if (!this->isInitialized())
        this->init();

    // Prepare attachment:
    uint32_t attId = static_cast<uint32_t>(reserved->attachment.size());
    Eng::Fbo::Attachment att;
    att.texture = texture;
    att.size = glm::u32vec2{texture.getSizeX(), texture.getSizeY()};

    glBindFramebuffer(GL_FRAMEBUFFER, reserved->oglId);
    switch (texture.getFormat())
    {
    /////////////////////////////////////
    case Eng::Texture::Format::r8g8b8: //
    case Eng::Texture::Format::r8g8b8a8:
        att.type = Eng::Fbo::Attachment::Type::color_texture;
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attId, GL_TEXTURE_2D, texture.getOglHandle(), 0);
        break;

    ////////////////////////////////////
    case Eng::Texture::Format::depth: //
        att.type = Eng::Fbo::Attachment::Type::depth_texture;
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture.getOglHandle(), 0);
        break;

    ///////////
    default: //
        ENG_LOG_ERROR("Unsupported texture format");
        return false;
    }

    // Done:   
    reserved->attachment.push_back(att);
    return updateMrtCache();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Add a depth render buffer. 
 * @param sizeX width
 * @param sizeY height
 * @return TF
 */
bool ENG_API Eng::Fbo::attachDepthBuffer(uint32_t sizeX, uint32_t sizeY)
{
    // Lazy-load on first attachment:
    if (!this->isInitialized())
        this->init();

    // Prepare attachment:
    uint32_t attId = static_cast<uint32_t>(reserved->attachment.size());
    Eng::Fbo::Attachment att;
    att.type = Eng::Fbo::Attachment::Type::depth_buffer;
    att.size = glm::u32vec2{sizeX, sizeY};

    // Generate render buffer:
    GLuint oglId = 0;
    glGenRenderbuffers(1, &oglId);
    glBindRenderbuffer(GL_RENDERBUFFER, oglId);

    // Attach renderbuffer:
    glBindFramebuffer(GL_FRAMEBUFFER, reserved->oglId);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32F, sizeX, sizeY);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, oglId);

    // Done:   
    att.data = oglId;
    reserved->attachment.push_back(att);
    return updateMrtCache();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	 
/** 
 * Update the MRT cache. 
 * @return TF
 */
bool ENG_API Eng::Fbo::updateMrtCache()
{
    bool oneAtLeast = false;
    reserved->mrt.clear();
    for (uint32_t c = 0; c < this->getNrOfAttachments(); c++)
        switch (reserved->attachment.at(c).type)
        {
        //////////////////////////////////////////////////
        case Eng::Fbo::Attachment::Type::color_texture: //         
            reserved->mrt.push_back(GL_COLOR_ATTACHMENT0 + c);
            oneAtLeast = true;
            break;
        }

    // No color attachments?
    if (!oneAtLeast)
    {
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    }

    // Done: 
    return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	 
/** 
 * Validate the state of the framebuffer.
 * @return TF
 */
bool ENG_API Eng::Fbo::validate() const
{
    // Additional check (for attachments of different size, which I think are ok according to the specs):
    bool throwWarning = false;
    if (this->getNrOfAttachments() > 1)
        for (uint32_t c = 1; c < this->getNrOfAttachments(); c++)
            if (reserved->attachment.at(c).size != reserved->attachment.at(0).size)
                throwWarning = true;
    if (throwWarning)
        ENG_LOG_WARN("Attachments have different size");

    glBindFramebuffer(GL_FRAMEBUFFER, reserved->oglId);
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        ENG_LOG_ERROR("Framebuffer not complete (error: %u)", status);
        return false;
    }

    // Done:
    return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Detach framebuffer: rendering is done on the main context buffers. 
 * @param viewportSizeX width of the viewport
 * @param viewportSizeY height of the viewport
 */
void ENG_API Eng::Fbo::reset(uint32_t viewportSizeX, uint32_t viewportSizeY)
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, viewportSizeX, viewportSizeY);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Blits directly from FBO. FBO and main buffer size must match. 
 * @param viewportSizeX width of the viewport
 * @param viewportSizeY height of the viewport
 * @return TF
 */
bool ENG_API Eng::Fbo::blit(uint32_t viewportSizeX, uint32_t viewportSizeY, bool invertOrder, bool depthBuffer) const
{
    if (invertOrder)
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, reserved->oglId);
    }
    else
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, reserved->oglId);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    }

    if (depthBuffer)
        glBlitFramebuffer(0, 0, getSizeX(), getSizeY(),
                          0, 0, viewportSizeX, viewportSizeY,
                          GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    else
        glBlitFramebuffer(0, 0, getSizeX(), getSizeY(),
                          0, 0, viewportSizeX, viewportSizeY,
                          GL_COLOR_BUFFER_BIT, GL_NEAREST);


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
bool ENG_API Eng::Fbo::render(uint32_t value, void* data) const
{
    // Safety net:
    if (this->getNrOfAttachments() == 0)
    {
        ENG_LOG_ERROR("No attachments available");
        return false;
    }

    // Bind buffers:
    glBindFramebuffer(GL_FRAMEBUFFER, reserved->oglId);
    const GLsizei nrOfMrts = static_cast<GLsizei>(reserved->mrt.size());
    if (nrOfMrts)
        glDrawBuffers(nrOfMrts, reserved->mrt.data());
    glViewport(0, 0, getSizeX(), getSizeY());

    // Done:
    return true;
}
