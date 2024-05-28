#pragma once

#include "engine.h"


namespace Eng
{
    class ENG_API Acbo : public Eng::Object, public Eng::Managed
    {
    public:
        static Acbo empty;

        // Const/dest:
        Acbo();
        Acbo(Acbo&& other);
        Acbo(Acbo const&) = delete;
        ~Acbo();

        // Get/set:   
        uint32_t getOglHandle() const;

        // data:
        bool create(uint32_t size, uint32_t format, uint32_t usage);
        bool create();

        // Rendering methods:
        static void reset();
        bool render(uint32_t value = 0, void* data = nullptr) const;

        // Managed:
        bool init() override;
        bool free() override;

    private: //

        // Reserved:
        struct Reserved;
        std::unique_ptr<Reserved> reserved;

        // Const/dest:
        Acbo(const std::string& name);
    };
};
