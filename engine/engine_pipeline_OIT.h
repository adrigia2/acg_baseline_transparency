#pragma once
#include "engine.h"

namespace Eng
{

    class ENG_API PipelineOIT final : public Eng::Pipeline
    {
    public:
        PipelineOIT();
        PipelineOIT(PipelineOIT&& other);
        PipelineOIT(PipelineOIT const&) = delete;
        ~PipelineOIT();

        // Get/set:
        void setWireframe(bool flag);
        bool isWireframe() const;

        // rendering methods:
        bool render(const Eng::Camera& camera, const Eng::List& list) override;
        bool render(const glm::mat4& camera, const glm::mat4& proj, const Eng::List& list) override;

        void pass1(const glm::mat4& camera, const glm::mat4& proj, const Eng::List& list);
        void pass2();

        // Managed:
        bool init() override;
        void clearBuffers();
        bool free() override;


    private:
        struct Reserved;
        std::unique_ptr<Reserved> reserved;

        PipelineOIT(const std::string& name);
    };
}
