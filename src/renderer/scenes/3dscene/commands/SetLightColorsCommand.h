#include "Command.h"
#include "Light.h"
#include <memory>

class SetLightColorsCommand : public Command
{
public:
    explicit SetLightColorsCommand(std::shared_ptr<Light> light, ofFloatColor newDiffuseColor, ofFloatColor newSpecularColor, ofFloatColor newAmbientColor) : light(std::move(light)), newDiffuseColor(newDiffuseColor), newSpecularColor(newSpecularColor), newAmbientColor(newAmbientColor)
    {
        this->oldDiffuseColor = this->light->GetDiffuseColor();
        this->oldSpecularColor = this->light->GetSpecularColor();
        this->oldAmbientColor = this->light->GetAmbientColor();

        this->name = "Set light colors to: diffuse(" + std::to_string(newDiffuseColor.r) + ", " + std::to_string(newDiffuseColor.g) + ", " + std::to_string(newDiffuseColor.b) + ", " + std::to_string(newDiffuseColor.a) + "), specular(" + std::to_string(newSpecularColor.r) + ", " + std::to_string(newSpecularColor.g) + ", " + std::to_string(newSpecularColor.b) + ", " + std::to_string(newSpecularColor.a) + "), ambient(" + std::to_string(newAmbientColor.r) + ", " + std::to_string(newAmbientColor.g) + ", " + std::to_string(newAmbientColor.b) + ", " + std::to_string(newAmbientColor.a) + ")";
    }

    SetLightColorsCommand(std::shared_ptr<Light> light, ofFloatColor newDiffuseColor, ofFloatColor newSpecularColor, ofFloatColor newAmbientColor, ofFloatColor oldDiffuseColor, ofFloatColor oldSpecularColor, ofFloatColor oldAmbientColor) : light(std::move(light)), newDiffuseColor(newDiffuseColor), newSpecularColor(newSpecularColor), newAmbientColor(newAmbientColor), oldDiffuseColor(oldDiffuseColor), oldSpecularColor(oldSpecularColor), oldAmbientColor(oldAmbientColor)
    {
        this->name = "Set light colors to: diffuse(" + std::to_string(newDiffuseColor.r) + ", " + std::to_string(newDiffuseColor.g) + ", " + std::to_string(newDiffuseColor.b) + ", " + std::to_string(newDiffuseColor.a) + "), specular(" + std::to_string(newSpecularColor.r) + ", " + std::to_string(newSpecularColor.g) + ", " + std::to_string(newSpecularColor.b) + ", " + std::to_string(newSpecularColor.a) + "), ambient(" + std::to_string(newAmbientColor.r) + ", " + std::to_string(newAmbientColor.g) + ", " + std::to_string(newAmbientColor.b) + ", " + std::to_string(newAmbientColor.a) + ")";
    }

    bool Execute() override
    {
        light->SetDiffuseColor(newDiffuseColor);
        light->SetSpecularColor(newSpecularColor);
        light->SetAmbientColor(newAmbientColor);
        return true;
    }

    void Undo() override
    {
        light->SetDiffuseColor(oldDiffuseColor);
        light->SetSpecularColor(oldSpecularColor);
        light->SetAmbientColor(oldAmbientColor);
    }
    [[nodiscard]] std::string DisplayName() const override { return this->name; }

private:
    std::string name;

    std::shared_ptr<Light> light;


    ofFloatColor newDiffuseColor;
    ofFloatColor newSpecularColor;
    ofFloatColor newAmbientColor;

    ofFloatColor oldDiffuseColor;
    ofFloatColor oldSpecularColor;
    ofFloatColor oldAmbientColor;
};
