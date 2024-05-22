/**
 * @file		engine_camera.h
 * @brief	A simple camera with some extras
 *
 * @author	Achille Peternier (achille.peternier@supsi.ch), (C) SUPSI
 */
#pragma once



/**
 * @brief Class for modeling a camera.
 */
class ENG_API Camera final : public Eng::Node
{
//////////
public: //
//////////

   // Special values:
   static Camera empty;   

   // Const/dest:
   Camera();
   Camera(Camera &&other);
   Camera(Camera const &) = delete;
   ~Camera();   

   // Operators:
   void operator=(Camera const&) = delete;

   // Projection matrix:
   void setProjMatrix(const glm::mat4 &matrix);
   const glm::mat4 &getProjMatrix() const;
   
   // Get/set:   
   glm::vec3 getEye() const;   

   // Orbital camera:
   void rotateAzimuth(float degrees);
   void rotatePolar(float degrees);
   void zoom(float by);
   void lookAt(const Eng::Node &target);
   void update();

   // Rendering methods:   
   bool render(uint32_t value = 0, void *data = nullptr) const;

   // Cache:
   static Camera &getCached();


///////////
private: //
///////////

   // Cache:
   static std::reference_wrapper<Eng::Camera> cache;

   // Reserved:
   struct Reserved;
   std::unique_ptr<Reserved> reserved;

   // Const/dest:
   Camera(const std::string &name);
};





