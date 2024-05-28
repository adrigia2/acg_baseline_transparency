#pragma once
#include <GL/glew.h>

#include "engine.h"

namespace Eng
{

    enum BufferNames {
        COUNTER_BUFFER = 0,
        LINKED_LIST_BUFFER
      };

    struct ListNode {
        glm::vec4 color;
        GLfloat depth;
        GLuint next;
    };
    
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
        void ClearBuffers();
        bool free() override;

        GLuint buffers[2], fsQuad, headPtrTex;
        GLuint pass1Index, pass2Index;
        GLuint clearBuf;
        

    private:
        struct Reserved;
        std::unique_ptr<Reserved> reserved;

        PipelineOIT(const std::string& name);
    };
}
