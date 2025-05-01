#pragma once

#include "EScreens.hpp"

class IScreen {
public:
    IScreen() = default;
    virtual ~IScreen() = default;
    
    virtual void SetupSprites() {}

    virtual bool Load() = 0;
    virtual EScreens Update(float deltaTime) = 0; 
    virtual void Render() = 0;
    virtual void Shutdown() = 0;

protected:
    EScreens mNextScreen = EScreens::None;
};
