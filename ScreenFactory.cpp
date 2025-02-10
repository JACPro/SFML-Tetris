#include "ScreenFactory.hpp"
#include "GameScreen.hpp"

#include "Window.hpp"
#include "World.hpp"

ScreenFactory::ScreenFactory(Window& window, World& world) 
	: mWindow(window)
	, mWorld(world) 
{
}

std::unique_ptr<IScreen> ScreenFactory::BuildScreen(Screens screen) {
	mNewScreen = nullptr;

	switch (screen) {
	case Screens::MainMenu:
		// TODO 
		// NewScreen = std::make_unique<MainMenuScreen>();
		break;
	case Screens::Game:
		mNewScreen = std::make_unique<GameScreen>(mWindow, mWorld);
		break;
	case Screens::GameOver:
		// TODO 
		// NewScreen = std::make_unique<GameOverScreen>();
		break;
	}

	return std::move(mNewScreen);
	//return nullptr;
}
