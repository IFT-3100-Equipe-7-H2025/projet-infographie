#pragma once
#include "Command.h"
#include "Camera.h"

class SetCameraFovCommand : public Command
{
public:
    explicit SetCameraFovCommand(std::shared_ptr<Camera> camera, float newFov) : camera(std::move(camera))
                                                                                 , newFov(newFov)
    {
        this->oldFov = this->camera->getFov();

        this->name = "Set camera FOV to " + std::to_string(this->newFov);
    }

    SetCameraFovCommand(std::shared_ptr<Camera> camera, float newFov, float oldFov) : camera(std::move(camera))
                                                                                      , newFov(newFov)
                                                                                      , oldFov(oldFov)
    {
        this->name = "Set camera FOV to " + std::to_string(this->newFov);
    }

    bool Execute() override
    {
        this->camera->setFov(this->newFov);
        return true;
    }

    void                      Undo() override { this->camera->setFov(this->oldFov); }
    [[nodiscard]] std::string DisplayName() const override { return this->name; }

private:
    std::string name;

    std::shared_ptr<Camera> camera;

    float newFov;
    float oldFov;
};
