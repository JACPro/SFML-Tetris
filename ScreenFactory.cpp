#include "ScreenFactory.hpp"

#include "GameScreen.hpp"
#include "MainMenuScreen.hpp"
#include "GameOverScreen.hpp"

#include "Window.hpp"
#include "World.hpp"

ScreenFactory::ScreenFactory(Window& window, World& world) 
	: mWindow(window)
	, mWorld(world) 
{
}

std::unique_ptr<IScreen> ScreenFactory::BuildScreen(EScreens screen) {
	mNewScreen = nullptr;

	switch (screen) {
	case EScreens::MainMenu:
		mNewScreen = std::make_unique<MainMenuScreen>(mWindow, mWorld);
		break;
	case EScreens::Game:
		mNewScreen = std::make_unique<GameScreen>(mWindow, mWorld);
		break;
	case EScreens::GameOver:
		mNewScreen = std::make_unique<GameOverScreen>(mWindow, mWorld);
		break;
	}

	return std::move(mNewScreen);
}
