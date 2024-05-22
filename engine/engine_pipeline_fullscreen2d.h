
/**
 * @file		engine_pipeline_fullscreen2d.h
 * @brief	A pipeline for rendering a texture fullscreen in 2D
 *
 * @author	Achille Peternier (achille.peternier@supsi.ch), (C) SUPSI
 */
#pragma once



/**
 * @brief Fullscreen rendering 2D.
 */
class ENG_API PipelineFullscreen2D final : public Eng::Pipeline
{
//////////
public: //
//////////
   
   // Const/dest:
	PipelineFullscreen2D();      
	PipelineFullscreen2D(PipelineFullscreen2D &&other);
   PipelineFullscreen2D(PipelineFullscreen2D const&) = delete;   
   ~PipelineFullscreen2D(); 

   // Rendering methods:      
   bool render(const Eng::Texture &texture, const Eng::List &list);
   
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
   PipelineFullscreen2D(const std::string &name);
};






