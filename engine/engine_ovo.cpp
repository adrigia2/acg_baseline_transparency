/**
 * @file		engine_ovo.cpp
 * @brief	OVO importing
 *
 * @author	Achille Peternier (achille.peternier@supsi.ch), (C) SUPSI
 */



//////////////
// #INCLUDE //
//////////////

   // Main include:
   #include "engine.h"
   #include <functional>
   #include <map>

   // GLM:
   #include <glm/gtc/packing.hpp>  



///////////////////////
// BODY OF CLASS Ovo //
///////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Loads the specific information of a given object. In its base class, this function loads the file version chunk.
 * @param serializer serial data
 * @param data optional pointer
 * @return version or 0 if error
 */
uint32_t ENG_API Eng::Ovo::loadChunk(Eng::Serializer &serial, void *data)
{
   uint32_t chunkId;
   serial.deserialize(chunkId);

   if (chunkId != static_cast<uint32_t>(Ovo::ChunkId::version))
   {
      ENG_LOG_ERROR("Invalid chunk ID found");
      return 0;
   }   
   
   uint32_t chunkSize;
   serial.deserialize(chunkSize);

   uint32_t version;
   serial.deserialize(version);
   if (version != Ovo::version)
   {
      ENG_LOG_ERROR("Invalid format version");
      return 0;
   }   

   // Done:   
   return version;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Discards the current chunk and updates the serializer to the next chunk.
 * @param serializer serial data
 * @return size of the ignored chunk
 */
uint32_t ENG_API Eng::Ovo::ignoreChunk(Eng::Serializer &serial)
{
   uint32_t chunkId;
   serial.deserialize(chunkId);
   
   uint32_t chunkSize;
   serial.deserialize(chunkSize);   

   std::vector<uint8_t> dummy(chunkSize);
   serial.deserialize(dummy.data(), chunkSize);

   // Done:   
   return chunkSize;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Loads an OVO file.
 * @param filename 3D file 
 * @return root node or Node::empty if error
 */
Eng::Node ENG_API &Eng::Ovo::load(const std::string &filename)
{
   // Safety net:
   if (filename.empty())
   {
      ENG_LOG_ERROR("Invalid params");
      return Eng::Node::empty;
   }


   /////////////////////////////////////////
   // STEP 1: load file into a memory buffer
   bool error = false;
   FILE *dat = fopen(filename.c_str(), "rb");
   if (dat == nullptr)
   {
      ENG_LOG_ERROR("Unable to open file '%s'", filename.c_str());
      return Eng::Node::empty;
   }

   // Get file length (max 2 GB):
   uint64_t length;
   fseek(dat, 0L, SEEK_END);
      length = ftell(dat);
   fseek(dat, 0L, SEEK_SET);

   // Init mem and copy:
   Eng::Serializer serial(nullptr, length);
   if (fread(serial.getData(), sizeof(uint8_t), length, dat) != length)
   {
      ENG_LOG_ERROR("File '%s' is corrupted", filename.c_str());
      fclose(dat);
      return Eng::Node::empty;
   }
   fclose(dat);  

   // First chunk must be the format version:   
   if (loadChunk(serial) == 0)
   {
      ENG_LOG_ERROR("Invalid format version or wrong file format for file '%s'", filename.c_str());
      return Eng::Node::empty;
   }
   

   ///////////////////////////////
   // STEP 2: Materials and geoms:  
   Eng::Container &container = Eng::Container::getInstance();
   std::function<Eng::Node& (void)> parse;
   parse = [&serial, &container, this, &parse, &error](void)->Eng::Node&
   {
      switch (*(static_cast<uint8_t *>(serial.getDataAtCurPos())))
      {
         ///////////////////////////////////////////////////////////
         case static_cast<uint32_t>(Eng::Ovo::ChunkId::material): //
         {
            ENG_LOG_DEBUG("Processing material...");

            Eng::Material mat;
            mat.loadChunk(serial);
            container.add(mat);
            return Eng::Node::empty;            
         }
         break;

         ///////////////////////////////////////////////////////
         case static_cast<uint32_t>(Eng::Ovo::ChunkId::node): //
         {
            ENG_LOG_DEBUG("Processing node...");

            Eng::Node node;
            uint32_t nrOfChildren = node.loadChunk(serial);            
            container.add(node);
            std::reference_wrapper<Eng::Node> _node = container.getLastNode();
            while (_node.get().getNrOfChildren() < nrOfChildren)
               _node.get().addChild(parse());              
            return _node;
         }
         break;

         ///////////////////////////////////////////////////////
         case static_cast<uint32_t>(Eng::Ovo::ChunkId::mesh): //
         {
            ENG_LOG_DEBUG("Processing mesh...");

            Eng::Mesh mesh;
            uint32_t nrOfChildren = mesh.loadChunk(serial);            
            container.add(mesh);
            std::reference_wrapper<Eng::Mesh> _mesh = container.getLastMesh();
            while (_mesh.get().getNrOfChildren() < nrOfChildren)
               _mesh.get().addChild(parse());              
            return _mesh;
         }
         break;

         ////////////////////////////////////////////////////////
         case static_cast<uint32_t>(Eng::Ovo::ChunkId::light): //
         {
            ENG_LOG_DEBUG("Processing light...");

            Eng::Light light;
            uint32_t nrOfChildren = light.loadChunk(serial);
            container.add(light);
            std::reference_wrapper<Eng::Light> _light = container.getLastLight();
            while (_light.get().getNrOfChildren() < nrOfChildren)
               _light.get().addChild(parse());              
            return _light;
         }
         break;

         ///////////
         default: //
            ENG_LOG_WARN("Unknown chunk ID (%u) found: ignored", *(static_cast<uint32_t *>(serial.getDataAtCurPos())));
            ignoreChunk(serial);
            return Eng::Node::empty;
      }
   };

   // Iterate:
   std::reference_wrapper<Eng::Node> root(Eng::Node::empty);
   while (serial.getDataAtCurPos() && !error)
      root = parse();

   // Done:   
   return root;
}
