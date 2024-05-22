/**
 * @file		engine_node.h
 * @brief	Basic hierarchy node properties
 *
 * @author	Achille Peternier (achille.peternier@supsi.ch), (C) SUPSI
 */
#pragma once



/**
 * @brief Class for modelling a generic node. 
 */
class ENG_API Node : public Eng::Object, public Eng::Ovo
{
//////////
public: //
//////////

   // Special values:
   static Node empty;          

   // Const/dest:
	Node();      
	Node(Node &&other);
   Node(Node const&) = delete;   
   virtual ~Node();      

   // Operators:
   void operator=(Node const&) = delete;  
   
   // Positioning:
   void setMatrix(const glm::mat4 &matrix);
   const glm::mat4 &getMatrix() const;
   glm::mat4 getWorldMatrix(Node &root = Node::empty) const;

   // Hierarchy:
   uint32_t getNrOfChildren() const;
   Node &getParent() const;
   bool addChild(Node &child);
   Node &getChild(uint32_t id) const;
   Node &removeChild(uint32_t id);   
   const std::list<std::reference_wrapper<Node>> &getListOfChildren() const;      

   // Ovo:   
   uint32_t loadChunk(Eng::Serializer &serial, void *data = nullptr) override;
   
   // Debugging:
   std::string getTreeAsString() const;
   

/////////////
protected: //
/////////////

   // Reserved:
   struct Reserved;           
   std::unique_ptr<Reserved> reserved;			

   // Const/dest:
   Node(const std::string &name);

   // Hierarchy:
   void setParent(Node &parent);
};



