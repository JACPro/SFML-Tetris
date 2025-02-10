#pragma once

class IScreen {
public:
    IScreen() = default;
    virtual ~IScreen() = default;
    virtual bool Load() = 0;
    virtual bool Update(float deltaTime) = 0; 
    virtual void Render() = 0;
    virtual void Shutdown() = 0;

protected:
    // TODO how know next screen?
};
