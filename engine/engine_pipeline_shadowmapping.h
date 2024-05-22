/**
 * @file		engine_pipeline_shadowmapping.h
 * @brief	A pipeline for generating planar shadow maps
 *
 * @author	Achille Peternier (achille.peternier@supsi.ch), (C) SUPSI
 */
#pragma once



/**
 * @brief Planar shadow mapping pipeline.
 */
class ENG_API PipelineShadowMapping final : public Eng::Pipeline
{
//////////
public: //
//////////

   // Special values:
   constexpr static uint32_t depthTextureSize = 2048;     ///< Size of the depth map

   
   // Const/dest:
	PipelineShadowMapping();      
	PipelineShadowMapping(PipelineShadowMapping &&other);
   PipelineShadowMapping(PipelineShadowMapping const&) = delete;   
   ~PipelineShadowMapping(); 

   // Get/set:
   const Eng::Texture &getShadowMap() const;

   // Rendering methods:   
   bool render(const Eng::Camera &camera, const Eng::List &list) override;
   bool render(const glm::mat4 &camera, const glm::mat4 &proj, const Eng::List &list) override;
   
   // Managed:
   bool init() override;
   bool free() override;


///////////
private: //
///////////

   // Reserved:
   struct Reserved;           
   std::unique_ptr<Reserved> reserved;			

   // Const/dest:
   PipelineShadowMapping(const std::string &name);
};






