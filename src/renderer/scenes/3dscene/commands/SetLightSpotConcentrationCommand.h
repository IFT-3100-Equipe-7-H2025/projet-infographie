#include "Command.h"
#include "Light.h"
#include <memory>

class SetLightSpotConcentrationCommand : public Command
{
public:
    explicit SetLightSpotConcentrationCommand(std::shared_ptr<Light> light, float newSpotConcentration) : light(std::move(light)), newSpotConcentration(newSpotConcentration)
    {
        this->oldSpotConcentration = this->light->GetSpotConcentration();

        this->name = "Setting light spot concentration: " + std::to_string(this->newSpotConcentration);
    }

    SetLightSpotConcentrationCommand(std::shared_ptr<Light> light, float newSpotConcentration, float oldSpotConcentration) : light(std::move(light)), newSpotConcentration(newSpotConcentration), oldSpotConcentration(oldSpotConcentration)
    {
        this->name = "Setting light spot concentration: " + std::to_string(this->newSpotConcentration);
    }

    bool Execute() override
    {
        this->light->SetSpotConcentration(this->newSpotConcentration);
        return true;
    }

    void Undo() override
    {
        this->light->SetSpotConcentration(this->oldSpotConcentration);
    }
    [[nodiscard]] std::string DisplayName() const override { return this->name; }

private:
    std::string name;

    std::shared_ptr<Light> light;

    float newSpotConcentration;

    float oldSpotConcentration;
};
