#include "World.hpp"

#include "Entity.hpp"

#include <sstream>

World::World(Window& window)
	: mWindow(window) 
	, mScreenFactory(window, *this)
{
}

bool World::Load() {
	if (!mResourceManager.Load()) {
		return false;
	}

	mCurrentScreen = mScreenFactory.BuildScreen(EScreens::MainMenu);
	
	if (!mCurrentScreen) { return false; }
	
	mCurrentScreen->Load();

	return true;
}


void World::Update(float deltaTime) {
	if (!mWindow.GetRenderWindow().hasFocus()) {
		// do not Update when window not in focus
		return;
	}

	EScreens nextScreen = mCurrentScreen->Update(deltaTime);

	if (nextScreen != EScreens::None) {
		mCurrentScreen = mScreenFactory.BuildScreen(nextScreen);
		mCurrentScreen->Load();
	}
}

void World::Render() {
	mCurrentScreen->Render();
}

void World::Shutdown() {
	mCurrentScreen->Shutdown();
}

