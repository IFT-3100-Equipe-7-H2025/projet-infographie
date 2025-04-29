#include "Command.h"
#include "Light.h"
#include <memory>

class EnableLightCommand : public Command
{
public:
    explicit EnableLightCommand(std::shared_ptr<Light> light, bool enabled) : light(std::move(light)), enabled(enabled)
    {
        std::string enable = enabled ? "Enable" : "Disable";
        this->name = enable + " light ";
    }

    bool Execute() override
    {
        if (enabled)
        {
            this->light->Enable();
        }
        else
        {
            this->light->Disable();
        }
        return true;
    }

    void Undo() override
    {
        if (enabled)
        {
            this->light->Disable();
        }
        else
        {
            this->light->Enable();
        }
    }
    [[nodiscard]] std::string DisplayName() const override { return this->name; }

private:
    std::string name;

    std::shared_ptr<Light> light;

    bool enabled;
};
