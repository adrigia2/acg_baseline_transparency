/**
 * @file		engine_list.h
 * @brief	List of objects after the scenegraph traversal
 *
 * @author	Achille Peternier (achille.peternier@supsi.ch), (C) SUPSI
 */
#pragma once



/**
 * @brief Class for storing a list of objects after the scenegraph traversal.
 */
class ENG_API List final : public Eng::Object
{
//////////
public: //
//////////

   // Special values:
   static List empty;   

   
   /**
    * @brief Types of rendering passes. 
    */
   enum class Pass : uint32_t
   {
      none,

      // Categories:
      all,      
      lights,
      meshes,
      transparents,

      // Terminator:
      last
   };


   /**
    * @brief Renderable element
    */
   struct RenderableElem
   {
      std::reference_wrapper<const Eng::Object> reference;  ///< Reference to the original object
      glm::mat4 matrix;                                     ///< Final position in world coordinates     


      /**
       * Constructor. 
       */
      RenderableElem() : reference{ Eng::Object::empty }, matrix{ 1.0f }
      {}
   };


   // Const/dest:
	List();      
	List(List &&other);
   List(List const&) = delete;   
   virtual ~List();         
   
   // Get/set:
   const std::vector<Eng::List::RenderableElem> &getRenderableElems() const;
   const Eng::List::RenderableElem &getRenderableElem(uint32_t elemNr) const;
   uint32_t getNrOfRenderableElems() const;
   uint32_t getNrOfLights() const;
     
   // Scene graph traversal:
   void reset();
   bool process(const Eng::Node &node, const glm::mat4 &prevMatrix = glm::mat4(1.0f));   

   // Rendering:   
   bool render(const Eng::Camera &camera, Pass pass = Pass::all) const;
   bool render(const glm::mat4 &cameraMatrix, const glm::mat4 &projectionMatrix, Pass pass = Pass::all) const;


///////////
private: //
///////////   

   // Reserved:
   struct Reserved;           
   std::unique_ptr<Reserved> reserved;			

   // Const/dest:
   List(const std::string &name);

   // Workaround for disabling the unneeded rendering method:
   using Object::render;
};
