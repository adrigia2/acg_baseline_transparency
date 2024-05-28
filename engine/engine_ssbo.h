/**
 * @file		engine_ssbo.h
 * @brief	OpenGL Shader Storage Buffer Object (SSBO)
 *
 * @author	Achille Peternier (achille.peternier@supsi.ch), (C) SUPSI
 */
#pragma once
#include <GL/glew.h>


namespace Eng
{
    /**
     * @brief Class for modeling a Shader Storage Buffer Object (SSBO).
     */
    class ENG_API Ssbo final : public Eng::Object, public Eng::Managed
    {
        //////////
    public: //
        //////////

        // Special values:
        static Ssbo empty;


        /**
         * @brief Types of mapping. 
         */
        enum class Mapping : uint32_t
        {
            read,
            write
        };


        // Const/dest:
        Ssbo();
        Ssbo(Ssbo&& other);
        Ssbo(Ssbo const&) = delete;
        ~Ssbo();

        // Get/set:   
        uint64_t getSize() const;
        uint32_t getOglHandle() const;

        // Data:
        bool create(uint64_t size, const void* data = nullptr);
        bool create(uint64_t size, const void* data, GLbitfield flags);
        void* map(Mapping mapping);
        bool unmap();

        // Rendering methods:   
        bool render(uint32_t value = 0, void* data = nullptr) const;

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
        Ssbo(const std::string& name);
    };
}
