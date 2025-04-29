#include "Command.h"
#include "Light.h"
#include <memory>

class SetLightTypeCommand : public Command
{
public:
    explicit SetLightTypeCommand(std::shared_ptr<Light> light, LightType newType) : light(std::move(light)), newType(newType)
    {
        this->oldType = this->light->GetLightType();

        this->name = "Set light type to " + this->TypeToString(this->newType);
    }

    SetLightTypeCommand(std::shared_ptr<Light> light, LightType newType, LightType oldType) : light(std::move(light)), newType(newType), oldType(oldType)
    {
        this->name = "Set light type to " + this->TypeToString(this->newType);
    }

    bool Execute() override
    {
        this->light->SetLightType(this->newType);
        return true;
    }

    void Undo() override { this->light->SetLightType(this->oldType); }
    [[nodiscard]] std::string DisplayName() const override { return this->name; }

private:
    [[nodiscard]] static std::string TypeToString(LightType type)
    {
        switch (type)
        {
            case LightType::POINT:
                return "POINT";
            case LightType::DIRECTIONAL:
                return "DIRECTIONAL";
            case LightType::SPOT:
                return "SPOT";
            case LightType::AMBIENT:
                return "AMBIENT";
        }
        return "UNKNOWN";
    }

    std::string name;

    std::shared_ptr<Light> light;

    LightType newType;
    LightType oldType;
};
