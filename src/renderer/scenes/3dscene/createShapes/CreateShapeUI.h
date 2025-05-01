#pragma once
#include "3dscene/commands/CommandHistory.h"
#include "SharedShapeCreationParams.h"
#include "imgui.h"

class CreateShapeUI
{
public:
    virtual ~CreateShapeUI() = default;
    CreateShapeUI(std::shared_ptr<SharedShapeCreationParams> sharedParams, CommandHistory& history) : sharedParams(std::move(sharedParams)), history(history) {}
    virtual void Draw() = 0;

    void SetPosition(float x, float y)
    {
        SetPositionX(x);
        SetPositionY(y);
    }

    void SetPositionX(float x) { positionX = x; }
    void SetPositionY(float y) { positionY = y; }
    [[nodiscard]] float GetPositionX() const { return positionX; }
    [[nodiscard]] float GetPositionY() const { return positionY; }

protected:
    float positionX = 0.0f;
    float positionY = 0.0f;

    std::shared_ptr<SharedShapeCreationParams> sharedParams;
    CommandHistory& history;
};
