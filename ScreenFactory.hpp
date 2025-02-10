#pragma once

#include "memory"

#include "Screen.hpp"
#include "EScreens.hpp"

class Window;
class World;

class ScreenFactory {
public:
    ScreenFactory(Window& window, World& world);

    std::unique_ptr<IScreen> BuildScreen(EScreens screen);

private:
    std::unique_ptr<IScreen> mNewScreen;

    Window& mWindow;
    World& mWorld;
};
