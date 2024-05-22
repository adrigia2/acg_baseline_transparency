/**
 * @file		engine_node.cpp
 * @brief	Basic generic node properties
 *
 * @author	Achille Peternier (achille.peternier@supsi.ch), (C) SUPSI
 */



//////////////
// #INCLUDE //
//////////////

   // Main include:
   #include "engine.h"

   // C/C++:
   #include <map>

   

////////////
// STATIC //
////////////

   // Special values:
   Eng::Node Eng::Node::empty("[empty]");      
   


/////////////////////////
// RESERVED STRUCTURES //
/////////////////////////

/**
 * @brief Node reserved structure.
 */
struct Eng::Node::Reserved
{  
   glm::mat4 matrix;                                                    ///< Node matrix
   std::reference_wrapper<Eng::Node> parent;                            ///< Parent node
   std::list<std::reference_wrapper<Eng::Node>> children;               ///< List of children nodes      


   /**
    * Constructor. 
    */
   Reserved() : matrix{ 1.0f },
                parent{ Eng::Node::empty }
   {}
};



////////////////////////
// BODY OF CLASS Node //
////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Constructor.
 */
ENG_API Eng::Node::Node() : reserved(std::make_unique<Eng::Node::Reserved>())
{		
   ENG_LOG_DETAIL("[+]");
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Constructor with name.
 * @param name node name
 */
ENG_API Eng::Node::Node(const std::string &name) : Eng::Object(name), reserved(std::make_unique<Eng::Node::Reserved>())
{	   
   ENG_LOG_DETAIL("[+]");
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Move constructor. 
 */
ENG_API Eng::Node::Node(Node &&other) : Eng::Object(std::move(other)), reserved(std::move(other.reserved))
{ 
   ENG_LOG_DETAIL("[M]");
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Destructor.
 */
ENG_API Eng::Node::~Node()
{	
   ENG_LOG_DETAIL("[-]");
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Sets the node matrix.
 * @param matrix glm mat4x4 
 */
void ENG_API Eng::Node::setMatrix(const glm::mat4 &matrix) 
{		
   reserved->matrix = matrix;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Gets the node matrix. 
 * @return glm 4x4 matrix
 */
const glm::mat4 ENG_API &Eng::Node::getMatrix() const
{	
   return reserved->matrix;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Gets the world coordinate matrix of this node starting from the specified node (if empty, root node is used)
 * @param node starting node (root if empty)
 * @return world coordinate glm 4x4 matrix
 */
glm::mat4 ENG_API Eng::Node::getWorldMatrix(Eng::Node &root) const
{	
   auto current = std::reference_wrapper<Eng::Node>(const_cast<Eng::Node &>(* this));   
   glm::mat4 result = glm::mat4(1.0f);

   // Back iteration:
   do 
   {
      result = result * glm::transpose(current.get().getMatrix());
      current = current.get().getParent();
   } while (current.get() != Eng::Node::empty && current.get() != root);
   
   // Done:
   return glm::transpose(result); 
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	 
/** 
 * Returns the parent node. 
 * @return parent node
 */	
Eng::Node ENG_API &Eng::Node::getParent() const
{	   
	return reserved->parent;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	 
/** 
 * Sets the parent node. 
 * @param parent parent node
 */	
void ENG_API Eng::Node::setParent(Eng::Node &parent)
{	   
	reserved->parent = parent;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	 
/** 
 * Returns a specific child, given its number.	 
 * @param child returns the child'th children of this node (range 0 to nrOfChildren - 1)
 * @return child reference or empty reference if none or error
 */	
Eng::Node ENG_API &Eng::Node::getChild(uint32_t id) const
{	
	// Safey net:
   if (id >= reserved->children.size())
	{
      ENG_LOG_ERROR("Invalid params");      
		return Node::empty;
	}		
	
	// Iterate up to the good one:	
	auto i = reserved->children.begin();				
	for (uint32_t c = 0; c < id; c++)	
		i++;		
	return i->get();		
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	 
/** 
 * Removes and returns a specific child, given its number.	 
 * @param child returns the child'th child of this node (range 0 to nrOfChildren - 1)
 * @return child reference or empty reference if none or error
 */
Eng::Node ENG_API &Eng::Node::removeChild(uint32_t id)
{	   
	// Safey net:
   if (id >= reserved->children.size())
	{
      ENG_LOG_ERROR("Invalid params");      
		return Eng::Node::empty;
	}		
	
	// Iterate up to the good one:	
	auto i = reserved->children.begin();				
	for (unsigned int c = 0; c < id; c++)	
		i++;

   // Remove and update:
   i->get().setParent(Eng::Node::empty);
   auto &x = i->get();
   reserved->children.erase(i);   
	return x;		
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	 
/** 
 * Adds a new child to this node. 
 * @param child child node to add 
 * @return TF
 */	
bool ENG_API Eng::Node::addChild(Eng::Node &child)
{  
	// Safety net:
	if (child == Node::empty)
	{
      ENG_LOG_ERROR("Invalid params");      
		return false;
	}		  

	// Already got a parent?
	if (child.getParent() != Node::empty)
	{
		ENG_LOG_ERROR("Child node already has a parent");      
		return false;
	}
	
	// Add and update:
   reserved->children.push_back(child);	
   child.setParent(*this);
   return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	 
/** 
 * Gets the number of children nodes.  
 * @return number of children nodes
 */	
uint32_t ENG_API Eng::Node::getNrOfChildren() const
{  
   return (uint32_t) reserved->children.size();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	 
/** 
 * Returns (as read-only) the internal list of children. This is used for perfomance reasons, to avoid iterating too much over the list.
 * @return reference to the internal list of children
 */	
const std::list<std::reference_wrapper<Eng::Node>> ENG_API &Eng::Node::getListOfChildren() const
{	   
   return reserved->children;	
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Loads the specific information of a given object. In its base class, this function loads the file version chunk.
 * @param serializer serial data
 * @param data optional pointer
 * @return TF
 */
uint32_t ENG_API Eng::Node::loadChunk(Eng::Serializer &serial, void *data)
{
   // Chunk header
   uint32_t chunkId;
   serial.deserialize(&chunkId, sizeof(uint32_t));
   if (chunkId != static_cast<uint32_t>(Ovo::ChunkId::node))
   {
      ENG_LOG_ERROR("Invalid chunk ID found");
      return 0;
   }
   uint32_t chunkSize;
   serial.deserialize(&chunkSize, sizeof(uint32_t));

   // Node properties:       
   std::string name;
   serial.deserialize(name);   
   this->setName(name);

   glm::mat4 matrix;
   serial.deserialize(matrix);

   uint32_t nrOfChildren;
   serial.deserialize(nrOfChildren);

   std::string target;
   serial.deserialize(target);   

   // Done:      
   return nrOfChildren;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	 
/** 
 * Gets a string representation of the hierarchy tree. For debugging purposes. 
 * @return multi-line string
 */	
std::string ENG_API Eng::Node::getTreeAsString() const
{     
   static std::string output = "";
   static unsigned int calls = 0;
   
   // Reset on first:
   if (calls == 0)
      output = "";

   // Update text:
   output += std::string(calls, ' ') + "+ " + this->getName();   
   output += '\n';   

   // Recursion:
   calls++;
      for (auto &i : reserved->children)
         i.get().getTreeAsString();
   calls--;
   
   // Done:
   if (calls == 0)
      return output;
   else
      return "";
}
