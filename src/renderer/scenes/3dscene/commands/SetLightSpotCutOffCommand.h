#include "Command.h"
#include "Light.h"
#include <memory>

class SetLightSpotCutOffCommand : public Command
{
public:
    explicit SetLightSpotCutOffCommand(std::shared_ptr<Light> light, float newSpotCutOff) : light(std::move(light)), newSpotCutOff(newSpotCutOff)
    {
        this->oldSpotCutOff = this->light->GetSpotlightCutOff();

        this->name = "Setting light spot cut off: " + std::to_string(this->newSpotCutOff);
    }

    SetLightSpotCutOffCommand(std::shared_ptr<Light> light, float newSpotCutOff, float oldSpotCutOff) : light(std::move(light)), newSpotCutOff(newSpotCutOff), oldSpotCutOff(oldSpotCutOff)
    {
        this->name = "Setting light spot cut off: " + std::to_string(this->newSpotCutOff);
    }

    bool Execute() override
    {
        this->light->SetSpotlightCutOff(this->newSpotCutOff);
        return true;
    }

    void Undo() override
    {
        this->light->SetSpotlightCutOff(this->oldSpotCutOff);
    }
    [[nodiscard]] std::string DisplayName() const override { return this->name; }

private:
    std::string name;

    std::shared_ptr<Light> light;

    float newSpotCutOff;

    float oldSpotCutOff;
};
