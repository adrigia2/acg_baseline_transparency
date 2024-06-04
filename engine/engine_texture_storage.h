#pragma once

#include <GL/glew.h>

#include "engine.h"

namespace Eng
{
    class ENG_API TextureStorage final : public Eng::Object, public Eng::Managed
    {
    public:
        static TextureStorage empty;

        TextureStorage();
        TextureStorage(TextureStorage&& other);
        TextureStorage(TextureStorage const&) = delete;
        virtual ~TextureStorage();

        // Get/set:
        uint32_t getSizeX() const;
        uint32_t getSizeY() const;
        uint32_t getOglHandle() const;
        uint64_t getOglBindlessHandle() const;


        bool create(uint32_t sizeX, uint32_t sizeY, uint32_t format);
        void reset();
        bool render(uint32_t value = 0, void* data = nullptr) const;

        // Managed:
        bool init() override;
        bool free() override;

    private:
        // Reserved:
        struct Reserved;
        std::unique_ptr<Reserved> reserved;

        // Const/dest:
        TextureStorage(const std::string& name);

        // Get/set:
        void setSizeX(uint32_t sizeX);
        void setSizeY(uint32_t sizeY);

        // Internal memory manager:   
        bool makeResident() const;
    };
}
