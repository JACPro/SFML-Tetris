#include "ResourceManager.hpp"

/*
* @brief Load all required resources from file into memory
* @return true if all resources were loaded successfully, false otherwise
*/
bool ResourceManager::Load() {
	bool success = true;

	success &= LoadTextures();
	success &= LoadFonts();

	return success;
}

bool ResourceManager::LoadTextures() {
	bool success = true;

	Textures& t = mTextures;

	success &= t.tiles.loadFromFile("Resources/Textures/Tiles.png");
	success &= t.background.loadFromFile("Resources/Textures/Background.png");
	t.background.setRepeated(true);
	success &= t.grid.loadFromFile("Resources/Textures/Grid.png");
	success &= t.frame.loadFromFile("Resources/Textures/Frame.png");
	success &= t.nextTileFrame.loadFromFile("Resources/Textures/NextTileFrame.png");

	return success;
}

bool ResourceManager::LoadFonts() {
	bool success = true;

	Fonts& f = mFonts;

	success &= f.font.loadFromFile("Resources/Fonts/Jersey10.ttf");

	return success;
}
