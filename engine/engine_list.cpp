/**
 * @file		engine_list.cpp
 * @brief	List of objects after the scenegraph traversal
 *
 * @author	Achille Peternier (achille.peternier@supsi.ch), (C) SUPSI
 */


//////////////
// #INCLUDE //
//////////////

// Main include:
#include "engine.h"


////////////
// STATIC //
////////////

// Special values:
Eng::List Eng::List::empty("[empty]");


/////////////////////////
// RESERVED STRUCTURES //
/////////////////////////

/**
 * @brief List reserved structure.
 */
struct Eng::List::Reserved
{
    std::vector<Eng::List::RenderableElem> renderableElem; ///< List of rendering elements
    uint32_t nrOfLights; ///< Number of lights in the list (lights come first)
    uint32_t nrOfSolidMeshes; ///< Number of solid meshes in the list (after lights)


    /**
     * Constructor. 
     */
    Reserved() : nrOfLights{0}, nrOfSolidMeshes{0}
    {
    }
};


////////////////////////
// BODY OF CLASS List //
////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Constructor.
 */
ENG_API Eng::List::List() : reserved(std::make_unique<Eng::List::Reserved>())
{
    ENG_LOG_DETAIL("[+]");
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Constructor with name.
 * @param name node name
 */
ENG_API Eng::List::List(const std::string& name) : Eng::Object(name), reserved(std::make_unique<Eng::List::Reserved>())
{
    ENG_LOG_DETAIL("[+]");
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Move constructor. 
 */
ENG_API Eng::List::List(List&& other) : Eng::Object(std::move(other)), reserved(std::move(other.reserved))
{
    ENG_LOG_DETAIL("[M]");
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Destructor.
 */
ENG_API Eng::List::~List()
{
    ENG_LOG_DETAIL("[-]");
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Resets internal list.
 */
void ENG_API Eng::List::reset()
{
    reserved->renderableElem.clear();
    reserved->nrOfLights = 0;
    reserved->nrOfSolidMeshes = 0;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Gets the number of currently loaded renderable elements. 
 * @return number of loaded renderable elements
 */
uint32_t ENG_API Eng::List::getNrOfRenderableElems() const
{
    return static_cast<uint32_t>(reserved->renderableElem.size());
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Gets the number of lights currently loaded in the list.
 * @return number of loaded lights
 */
uint32_t ENG_API Eng::List::getNrOfLights() const
{
    return reserved->nrOfLights;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Gets internal list of renderable elements.
 * @return list of renderable elements
 */
const std::vector<Eng::List::RenderableElem> ENG_API& Eng::List::getRenderableElems() const
{
    return reserved->renderableElem;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Gets a reference to the specified element in the list. 
 * @return element at the given position
 */
const Eng::List::RenderableElem ENG_API& Eng::List::getRenderableElem(uint32_t elemNr) const
{
    return reserved->renderableElem.at(elemNr);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Recursively parses the scenegraph starting at the given node and append the parsed elements to this list. 
 * @param node starting node
 * @param prevMatrix previous node matrix
 * @return TF
 */
bool ENG_API Eng::List::process(const Eng::Node& node, const glm::mat4& prevMatrix)
{
    // Safety net:
    if (node == Eng::Node::empty)
    {
        ENG_LOG_ERROR("Invalid params");
        return false;
    }

    RenderableElem re;
    re.matrix = prevMatrix * node.getMatrix();
    re.reference = node;

    // Store only renderable elements:
    if (dynamic_cast<const Eng::Light*>(&node)) // Lights first
    {
        reserved->renderableElem.insert(reserved->renderableElem.begin(), 1, re);
        reserved->nrOfLights++;
    }
    else if (dynamic_cast<const Eng::Mesh*>(&node)) // Only meshes
    {
        //reserved->renderableElem.insert(reserved->renderableElem.begin() + reserved->nrOfLights, 1, re);
        //reserved->renderableElem.push_back(re);

        
        if (const auto mesh = dynamic_cast<const Eng::Mesh*>(&node); mesh->getMaterial().getOpacity() < 1.0f)
        {
            reserved->renderableElem.push_back(re);
        }
        else
        {
            reserved->nrOfSolidMeshes++;
            auto iter = reserved->renderableElem.begin() + reserved->nrOfLights;
            reserved->renderableElem.insert(iter, 1, re);
        }
    }

    // Parse hierarchy recursively:
    for (auto& n : node.getListOfChildren())
        if (process(n, re.matrix) == false)
            return false;

    // Done:
    return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Parses the list and calls the render method of each renderable.
 * @param cameraMatrix camera (also view) matrix (must be already inverted)
 * @param projectionMatrix projection matrix
 * @param pass type of pass
 * @return TF
 */
bool ENG_API Eng::List::render(const glm::mat4& cameraMatrix, const glm::mat4& projectionMatrix,
                               Eng::List::Pass pass) const
{
    // Define range:
    size_t startRange = 0;
    size_t endRange = reserved->renderableElem.size();

    // TODO set projection matrix in shader

    switch (pass)
    {
    //////////////////
    case Pass::all: //
        break;

    /////////////////////
    case Pass::lights: //  
        endRange = reserved->nrOfLights;
        break;

    /////////////////////
    case Pass::meshes: //
        startRange = reserved->nrOfLights;
        endRange = reserved->nrOfLights + reserved->nrOfSolidMeshes;
        break;

    //////////////////////////
    case Pass::transparents: //
        startRange = reserved->nrOfLights + reserved->nrOfSolidMeshes;
        break;
    }

    // Iterate through the range:
    for (size_t c = startRange; c < endRange; c++)
    {
        RenderableElem& re = reserved->renderableElem.at(c);
        glm::mat4 finalMatrix = cameraMatrix * re.matrix;
        re.reference.get().render(0, &finalMatrix);
    }

    // Done:
    return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Shortcut for using a camera instead of the explicit matrices.  
 * @param camera camera to use
 * @param pass type of pass
 * @return TF
 */
bool ENG_API Eng::List::render(const Eng::Camera& camera, Eng::List::Pass pass) const
{
    return this->render(glm::inverse(camera.getWorldMatrix()), camera.getProjMatrix());
}
