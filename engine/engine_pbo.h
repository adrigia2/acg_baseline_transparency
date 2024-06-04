#pragma once

namespace Eng
{
    class ENG_API Pbo final : public Eng::Object, public Eng::Managed
    {
    public:
        static Pbo empty;

        Pbo();
        Pbo(Pbo&& other);
        Pbo(Pbo const&) = delete;
        ~Pbo();

        uint32_t getOglHandle() const;

        bool create(std::vector<uint32_t>& data);

        void reset();
        bool render(uint32_t value = 0, void* data = nullptr) const;

        // Managed:
        bool init() override;
        bool free() override;

    private:
        struct Reserved;
        std::unique_ptr<Reserved> reserved;

        Pbo(const std::string& name);
    };
}
