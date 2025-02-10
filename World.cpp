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

	mCurrentScreen = mScreenFactory.BuildScreen(Screens::Game);
	mCurrentScreen->Load();

	return true;
}


void World::Update(float deltaTime) {
	mCurrentScreen->Update(deltaTime);
}

void World::Render() {
	mCurrentScreen->Render();
}

void World::Shutdown() {
	mCurrentScreen->Shutdown();
}

