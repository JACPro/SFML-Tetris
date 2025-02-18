#include <cmath>

#include "MainMenuScreen.hpp"

#include "KeyHandler.hpp"
#include "World.hpp"
#include "CONSTANTS_ScreenLayout.hpp"

MainMenuScreen::MainMenuScreen(Window& window, World& world) 
	: mWindow(window)
	, mWorld(world)
{
}

void MainMenuScreen::SetupSprites() {
	const ResourceManager& resourceManager = mWorld.GetResources();

	sf::Vector2u bgSize = resourceManager.GetTextures().background.getSize();
	mBackgroundTileSize = {
		static_cast<int>(ceil((float)ScreenLayout::SCREEN_WIDTH / bgSize.x) * bgSize.x),
		static_cast<int>(ceil((float)ScreenLayout::SCREEN_HEIGHT / bgSize.y) * bgSize.y)
	};

	// Create 4 copies of bg sprite for scrolling effect
	for (int i = 0; i < sizeof(mBackgroundSprites) / sizeof(mBackgroundSprites[0]); i++) {
		mBackgroundSprites[i] = sf::Sprite(resourceManager.GetTextures().background);
		mBackgroundSprites[i].setTextureRect({ 0, 0, mBackgroundTileSize.x, mBackgroundTileSize.y });
	}

	mBackgroundSprites[0].setPosition(0, 0);
	mBackgroundSprites[1].setPosition(mBackgroundTileSize.x, 0);
	mBackgroundSprites[2].setPosition(0, mBackgroundTileSize.y);
	mBackgroundSprites[3].setPosition(mBackgroundTileSize.x, mBackgroundTileSize.y);
}

bool MainMenuScreen::Load() {
	SetupSprites();

	// Input 
	mKeyHandlers[sf::Keyboard::Enter] = KeyHandler(0.1f, [&]() { mNextScreen = EScreens::Game; });
	
	return true;
}

EScreens MainMenuScreen::Update(float deltaTime) {
	// Update key handlers
	for (auto& [key, handler] : mKeyHandlers) {
		handler.Update(key, deltaTime);
	}

	// Update background tile positions
	for (sf::Sprite& bgSprite : mBackgroundSprites) {
		sf::Vector2f currPos = bgSprite.getPosition();
		currPos.x += deltaTime * mBackgroundScrollSpeed.x;
		currPos.y += deltaTime * mBackgroundScrollSpeed.y;

		if (currPos.x > bgSprite.getGlobalBounds().width) {
			currPos.x -= bgSprite.getGlobalBounds().width * 2;
		}

		if (currPos.y > bgSprite.getGlobalBounds().height) {
			currPos.y -= bgSprite.getGlobalBounds().height * 2;
		}

		bgSprite.setPosition(currPos.x, currPos.y);
	}

	return mNextScreen;
}

void MainMenuScreen::Render() {
	// Render background tiles
	for (sf::Sprite bgSprite : mBackgroundSprites) {
		mWindow.GetRenderWindow().draw(bgSprite);
	}

	// -- TITLE TEXT --
	sf::Text titleText;

	titleText.setString("TOITRIS");

	// Formatting
	titleText.setFont(mWorld.GetResources().GetFonts().font); // a font is required to make a text object
	titleText.setCharacterSize(156);
	titleText.setFillColor(sf::Color(33, 205, 52));
	
	titleText.setOutlineColor(sf::Color(131, 246, 140));
	titleText.setOutlineThickness(2.0f);

	sf::FloatRect titleTextBounds = titleText.getLocalBounds();

	sf::Vector2f titleTextPosition = sf::Vector2f(
		ScreenLayout::SCREEN_WIDTH / 2 - titleTextBounds.width / 2,
		ScreenLayout::SCREEN_HEIGHT / 2 - titleTextBounds.height / 2 - 150 // offset height up by 200px
	);
	titleText.setPosition(titleTextPosition);


	// -- PRESS ENTER TEXT --
	sf::Text pressEnterText;

	pressEnterText.setString("Press Enter");

	// Formatting
	pressEnterText.setFont(mWorld.GetResources().GetFonts().font); // a font is required to make a text object
	pressEnterText.setCharacterSize(96);
	pressEnterText.setFillColor(sf::Color(210, 107, 180));

	pressEnterText.setOutlineColor(sf::Color(247, 194, 222));
	pressEnterText.setOutlineThickness(2.0f);

	sf::FloatRect pressEnterTextBounds = pressEnterText.getLocalBounds();

	sf::Vector2f pressEnterTextPosition = sf::Vector2f(
		ScreenLayout::SCREEN_WIDTH / 2 - pressEnterTextBounds.width / 2,
		ScreenLayout::SCREEN_HEIGHT / 2 - pressEnterTextBounds.height / 2 + 20 // offset height up by 200px
	);
	pressEnterText.setPosition(pressEnterTextPosition);



	// -- RENDER --
	mWindow.GetRenderWindow().draw(titleText);
	mWindow.GetRenderWindow().draw(pressEnterText);
}

void MainMenuScreen::Shutdown() {
}
