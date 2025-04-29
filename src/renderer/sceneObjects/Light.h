#pragma once

#pragma once

#include "SceneObject.h"
#include "ofLight.h"
#include "ofMain.h"

enum class LightType
{
    POINT,
    DIRECTIONAL,
    SPOT,
    AMBIENT
};

class Light : public SceneObject
{
public:
    Light(ofLight light, LightType type)
    {
        this->light = std::make_shared<ofLight>(light);
        this->lightType = type;
        this->attenuationConstant = light.getAttenuationConstant();
        this->attenuationLinear = light.getAttenuationLinear();
        this->attenuationQuadratic = light.getAttenuationQuadratic();
    }

    void Enable()
    {
        light->enable();
    }

    void Disable()
    {
        light->disable();
    }

    bool GetIsEnabled() const { return light->getIsEnabled(); }

    void SetLightType(LightType type)
    {
        lightType = type;
        switch (type)
        {
            case LightType::DIRECTIONAL:
                light->setDirectional();
                light->setAmbientColor(ofFloatColor(0.0f));
                break;
            case LightType::SPOT:
                light->setSpotlight();
                light->setAmbientColor(ofFloatColor(0.0f));
                break;
            case LightType::POINT:
                light->setPointLight();
                light->setAmbientColor(ofFloatColor(0.0f));
                break;
            case LightType::AMBIENT:
                light->setPointLight();
                light->setAmbientColor(ofFloatColor(1.0f));
                light->setDiffuseColor(ofFloatColor(0.0f));
                light->setSpecularColor(ofFloatColor(0.0f));
                break;
        }
    }

    void SetDiffuseColor(const ofFloatColor& color)
    {
        light->setDiffuseColor(color);
    }

    ofFloatColor GetDiffuseColor() const { return light->getDiffuseColor(); }

    void SetSpecularColor(const ofFloatColor& color)
    {
        light->setSpecularColor(color);
    }

    ofFloatColor GetSpecularColor() const { return light->getSpecularColor(); }

    void SetAmbientColor(const ofFloatColor& color)
    {
        light->setAmbientColor(color);
    }

    ofFloatColor GetAmbientColor() const { return light->getAmbientColor(); }

    LightType GetLightType() const { return lightType; }

    float GetAttenuationConstant() const { return light->getAttenuationConstant(); }

    void SetAttenuationConstant(float constant)
    {
        this->attenuationConstant = constant;
        this->SetAttenuation();
    }
    float GetAttenuationLinear() const { return light->getAttenuationLinear(); }

    void SetAttenuationLinear(float linear)
    {
        this->attenuationLinear = linear;
        this->SetAttenuation();
    }
    float GetAttenuationQuadratic() const { return light->getAttenuationQuadratic(); }

    void SetAttenuationQuadratic(float quadratic)
    {
        this->attenuationQuadratic = quadratic;
        this->SetAttenuation();
    }

    float GetSpotlightCutOff() const { return light->getSpotlightCutOff(); }

    void SetSpotlightCutOff(float cutoff)
    {
        light->setSpotlightCutOff(cutoff);
    }

    float GetSpotConcentration() const { return light->getSpotConcentration(); }

    void SetSpotConcentration(float concentration)
    {
        light->setSpotConcentration(concentration);
    }

    void lookAt(const ofVec3f& target)
    {
        light->lookAt(target);
    }

    void setOrientation(const glm::quat& q) { light->setOrientation(q); }
    void setPosition(const glm::vec3& position) { light->setPosition(position); }
    [[nodiscard]] glm::vec3 getPosition() const { return light->getPosition(); }
    [[nodiscard]] glm::quat getOrientation() const { return light->getOrientationQuat(); }

    void customDraw() override
    {
        ofPushMatrix();
        light->draw();
        // light->setPosition(ofNode::getPosition());
        ofPopMatrix();
    }

    std::pair<ofVec3f, ofVec3f> getBoundingVertices() override
    {
        ofVec3f pos = light->getGlobalPosition();
        ofVec3f minVertex = ofVec3f(pos.x - 5, pos.y - 5, pos.z - 5);
        ofVec3f maxVertex = ofVec3f(pos.x + 5, pos.y + 5, pos.z + 5);

        return std::pair(minVertex, maxVertex);
    };

private:
    void SetAttenuation() { light->setAttenuation(attenuationConstant, attenuationLinear, attenuationQuadratic); }

    std::shared_ptr<ofLight> light;
    LightType lightType = LightType::POINT;
    float attenuationConstant;
    float attenuationLinear;
    float attenuationQuadratic;
};
