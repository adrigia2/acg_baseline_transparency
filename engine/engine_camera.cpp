/**
 * @file		engine_camera.cpp
 * @brief	A simple camera with some extras
 *
 * @author	Achille Peternier (achille.peternier@supsi.ch), (C) SUPSI
 */



//////////////
// #INCLUDE //
//////////////

   // Main include:
   #include "engine.h"

   // GLM:
   #include <glm/gtc/packing.hpp>  

   // OGL:      
   #include <GL/glew.h>
   #include <GLFW/glfw3.h>
   


////////////
// STATIC //
////////////

   // Special values:
   Eng::Camera Eng::Camera::empty("[empty]");

   // Cache:
   std::reference_wrapper<Eng::Camera> Eng::Camera::cache = Eng::Camera::empty;



/////////////////////////
// RESERVED STRUCTURES //
/////////////////////////

/**
 * @brief Camera class reserved structure.
 */
struct Eng::Camera::Reserved
{  
   glm::mat4 projMatrix;   ///< Projection matrix
      
   glm::vec3 up;        /// Up vector
   float radius;        /// Orbital radius
   float minRadius;     /// Minimum radius
   float maxRadius;     /// Maximum radius
   float azimuthAngle;  /// Azimuth angle (we use radians internally)
   float polarAngle;    /// Polar angle (we use radians internally)
   std::reference_wrapper<const Eng::Node> target; /// Center around this target


   /**
    * Constructor
    */
   Reserved() : projMatrix{ 1.0f }, 
                up{ 0.0f, 1.0f, 0.0f },
                radius{ 50.0f }, minRadius{ 1.0f }, maxRadius{ 100.0f },
                azimuthAngle{ -0.5f }, polarAngle{ 0.5f },
                target{ Eng::Node::empty }
   {}
};



//////////////////////////
// BODY OF CLASS Camera //
//////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Constructor.
 */
ENG_API Eng::Camera::Camera() : reserved(std::make_unique<Eng::Camera::Reserved>())
{	
   ENG_LOG_DETAIL("[+]");   
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Constructor with name.
 * @param name mesh name
 */
ENG_API Eng::Camera::Camera(const std::string &name) : Eng::Node(name), reserved(std::make_unique<Eng::Camera::Reserved>())
{	
   ENG_LOG_DETAIL("[+]");   
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Move constructor. 
 */
ENG_API Eng::Camera::Camera(Camera &&other) : Eng::Node(std::move(other)), reserved(std::move(other.reserved))
{  
   ENG_LOG_DETAIL("[M]");   
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Destructor.
 */
ENG_API Eng::Camera::~Camera()
{
   ENG_LOG_DETAIL("[-]");   
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Sets the camera's projection matrix.
 * @param matrix glm mat4x4
 */
void ENG_API Eng::Camera::setProjMatrix(const glm::mat4 &matrix)
{
   reserved->projMatrix = matrix;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Gets the camera's projection matrix.
 * @return glm 4x4 projection matrix
 */
const glm::mat4 ENG_API &Eng::Camera::getProjMatrix() const
{
   return reserved->projMatrix;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Sets the center of the orbit around a given target.
 * @param target target node, or empty node if not used
 */
void ENG_API Eng::Camera::lookAt(const Eng::Node &target)
{
   reserved->target = target;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Rotates camera around azimuth.
 * @param degrees rotation degrees
 */
void ENG_API Eng::Camera::rotateAzimuth(float degrees)
{
   reserved->azimuthAngle += glm::radians(degrees);

   // Keep angle between 0..2PI:
   constexpr float fullCircle = 2.0f * glm::pi<float>();
   reserved->azimuthAngle = fmodf(reserved->azimuthAngle, fullCircle);
   if (reserved->azimuthAngle < 0.0f)
      reserved->azimuthAngle = fullCircle + reserved->azimuthAngle;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Rotates camera around polar axis.
 * @param degrees rotation degrees
 */
void ENG_API Eng::Camera::rotatePolar(float radians)
{
   reserved->polarAngle += glm::radians(radians);

   // Keep angle from flipping:
   constexpr float polarCap = glm::pi<float>() / 2.0f - 0.001f;
   reserved->polarAngle = glm::clamp(reserved->polarAngle, -polarCap, polarCap);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Zooms by the given amount.
 * param by zoom by this amount
 */
void ENG_API Eng::Camera::zoom(float by)
{
   reserved->radius += by;
   reserved->radius = glm::clamp(reserved->radius, reserved->minRadius, reserved->maxRadius);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Updates the orbital camera. 
 */
void ENG_API Eng::Camera::update()
{
   // Not in orbital mode:
   if (reserved->target.get() == Eng::Node::empty)
      return;
   
   // Orbital camera around target?   
   const float sineAzimuth = glm::sin(reserved->azimuthAngle);
   const float cosineAzimuth = glm::cos(reserved->azimuthAngle);
   const float sinePolar = glm::sin(reserved->polarAngle);
   const float cosinePolar = glm::cos(reserved->polarAngle);

   glm::vec3 center = reserved->target.get().getWorldMatrix()[3];
   glm::vec3 eyePos = glm::vec3(center.x + reserved->radius * cosinePolar * cosineAzimuth,
                                center.y + reserved->radius * sinePolar,
                                center.z + reserved->radius * cosinePolar * sineAzimuth);

   // Update node matrix:
   // @TODO: we should probably remove the parent's node final matrix here
   //        if the camera is in a hierarchy
   glm::mat4 lookAtMat = glm::lookAt(eyePos, center, reserved->up);
   lookAtMat = glm::inverse(lookAtMat);
   this->setMatrix(lookAtMat);   
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Get the last rendered camera.
 * @return last rendered camera
 */
Eng::Camera ENG_API &Eng::Camera::getCached()
{
   return cache;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Rendering method. 
 * @param value generic value
 * @param data generic pointer to any kind of data
 * @return TF
 */
bool ENG_API Eng::Camera::render(uint32_t value, void *data) const
{	
   Eng::Program &program = dynamic_cast<Eng::Program &>(Eng::Program::getCached());
   program.setMat4("projectionMat", reserved->projMatrix);
   
   // Done:
   Eng::Camera::cache = const_cast<Eng::Camera &>(*this);
   return true;
}
