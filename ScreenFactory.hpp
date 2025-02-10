#pragma once

#include "Screen.hpp"
#include "memory"

class Window;
class World;

enum class Screens {
    MainMenu,
    Game,
    GameOver
};

class ScreenFactory {
public:
    ScreenFactory(Window& window, World& world);

    std::unique_ptr<IScreen> BuildScreen(Screens screen);

private:
    std::unique_ptr<IScreen> mNewScreen;

    Window& mWindow;
    World& mWorld;
};
