#include "Command.h"
#include "Light.h"
#include <memory>
class SetLightAttenuationCommand : public Command
{
public:
    explicit SetLightAttenuationCommand(std::shared_ptr<Light> light, const glm::vec3& newAttenuation) : light(std::move(light)), newAttenuation(newAttenuation)
    {
        this->oldAttenuation = glm::vec3(this->light->GetAttenuationConstant(), this->light->GetAttenuationLinear(), this->light->GetAttenuationQuadratic());

        this->name = "Set light attenuation to: " + std::to_string(newAttenuation.x) + ", " + std::to_string(newAttenuation.y) + ", " + std::to_string(newAttenuation.z);
    }

    SetLightAttenuationCommand(std::shared_ptr<Light> light, const glm::vec3& newAttenuation, const glm::vec3& oldAttenuation) : light(std::move(light)), newAttenuation(newAttenuation), oldAttenuation(oldAttenuation)
    {
        this->name = "Set light attenuation to: " + std::to_string(newAttenuation.x) + ", " + std::to_string(newAttenuation.y) + ", " + std::to_string(newAttenuation.z);
    }

    bool Execute() override
    {
        light->SetAttenuationConstant(newAttenuation[0]);
        light->SetAttenuationLinear(newAttenuation[1]);
        light->SetAttenuationQuadratic(newAttenuation[2]);
        return true;
    }

    void Undo() override
    {
        light->SetAttenuationConstant(oldAttenuation[0]);
        light->SetAttenuationLinear(oldAttenuation[1]);
        light->SetAttenuationQuadratic(oldAttenuation[2]);
    }
    [[nodiscard]] std::string DisplayName() const override { return this->name; }

private:
    std::string name;

    std::shared_ptr<Light> light;


    glm::vec3 newAttenuation;
    glm::vec3 oldAttenuation;
};
