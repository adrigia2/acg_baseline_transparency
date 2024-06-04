#include "engine.h"
#include "engine_acbo.h"

// OGL:      
#include <GL/glew.h>
#include <GLFW/glfw3.h>



// static
Eng::Acbo Eng::Acbo::empty("[empty]");


/**
 * @brief Acbo reserved structure.
 */
struct Eng::Acbo::Reserved
{  
    GLuint oglId;        ///< OpenGL shader ID


    /**
     * Constructor.
     */
    Reserved() : oglId{ 0 }
    {}
};

ENG_API Eng::Acbo::Acbo() : reserved(std::make_unique<Eng::Acbo::Reserved>())
{
    ENG_LOG_DETAIL("[+]");
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Constructor with name.
 * @param name node name
 */
ENG_API Eng::Acbo::Acbo(const std::string &name) : Eng::Object(name), reserved(std::make_unique<Eng::Acbo::Reserved>())
{
    ENG_LOG_DETAIL("[+]");
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Move constructor.
 */
ENG_API Eng::Acbo::Acbo(Acbo &&other) : Eng::Object(std::move(other)), Eng::Managed(std::move(other)), reserved(std::move(other.reserved))
{
    ENG_LOG_DETAIL("[M]");
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Destructor.
 */
ENG_API Eng::Acbo::~Acbo()
{
    ENG_LOG_DETAIL("[-]");
    if (reserved)
        this->free();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Return the GLuint shader ID.
 * @return shader ID or 0 if not valid
 */
uint32_t ENG_API Eng::Acbo::getOglHandle() const
{
    return reserved->oglId;
}

bool Eng::Acbo::create(uint32_t size, void* data, uint32_t usage)
{
    glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, reserved->oglId);
    glBufferData(GL_ATOMIC_COUNTER_BUFFER, size, data, usage);
    return true;
}

bool Eng::Acbo::create()
{
    glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, reserved->oglId);
    glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), NULL, GL_DYNAMIC_DRAW);
    return true;
}


bool ENG_API Eng::Acbo::init()
{
    if (this->Eng::Managed::init() == false)
        return false;

    // Free buffer if already stored:
    if (reserved->oglId)
    {
        glDeleteBuffers(1, &reserved->oglId);
        reserved->oglId = 0;
    }

    // Create it:		       
    glGenBuffers(1, &reserved->oglId);

    // Done:   
    return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Releases an OpenGL Acbo.
 * @return TF
 */
bool ENG_API Eng::Acbo::free()
{
    if (this->Eng::Managed::free() == false)
        return false;

    // Free VAO if stored:
    if (reserved->oglId)
    {
        glDeleteBuffers(1, &reserved->oglId);
        reserved->oglId = 0;
    }

    // Done:   
    return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Unbind any vertex array object.  
 */
void ENG_API Eng::Acbo::reset()
{
    GLuint zero = 0;
    glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, reserved->oglId);
    glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &zero);

}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Rendering method. 
 * @param value generic value
 * @param data generic pointer to any kind of data
 * @return TF
 */
bool ENG_API Eng::Acbo::render(uint32_t value, void *data) const
{	   
    glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, value, reserved->oglId);
   
    // Done:
    return true;
}