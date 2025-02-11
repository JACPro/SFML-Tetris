#include <cmath>

#include "GameOverScreen.hpp"

#include "KeyHandler.hpp"
#include "World.hpp"
#include "CONSTANTS_ScreenLayout.hpp"

GameOverScreen::GameOverScreen(Window& window, World& world)
	: mWindow(window)
	, mWorld(world) {
}

void GameOverScreen::SetupSprites() {
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

bool GameOverScreen::Load() {
	SetupSprites();

	// Input 
	keyHandlers[sf::Keyboard::Enter] = KeyHandler(0.1f, [&]() { mNextScreen = EScreens::Game; });

	return true;
}

EScreens GameOverScreen::Update(float deltaTime) {
	// Update key handlers
	for (auto& [key, handler] : keyHandlers) {
		handler.update(key, deltaTime);
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

void GameOverScreen::Render() {
	// Render background tiles
	for (sf::Sprite bgSprite : mBackgroundSprites) {
		mWindow.GetRenderWindow().draw(bgSprite);
	}

	// -- TITLE TEXT --
	sf::Text titleText;

	titleText.setString("GAME OVER");

	// Formatting
	titleText.setFont(mWorld.GetResources().GetFonts().font); // a font is required to make a text object
	titleText.setCharacterSize(140);
	titleText.setFillColor(sf::Color(78, 70, 123));

	titleText.setOutlineColor(sf::Color(130, 230, 250));
	titleText.setOutlineThickness(2.0f);

	sf::FloatRect titleTextBounds = titleText.getLocalBounds();

	sf::Vector2f titleTextPosition = sf::Vector2f(
		ScreenLayout::SCREEN_WIDTH / 2 - titleTextBounds.width / 2,
		ScreenLayout::SCREEN_HEIGHT / 2 - titleTextBounds.height / 2 - 150 // offset height up by 200px
	);
	titleText.setPosition(titleTextPosition);



	// -- PRESS ENTER TEXT --
	sf::Text pressEnterText;

	pressEnterText.setString("Enter To Retry");

	// Formatting
	pressEnterText.setFont(mWorld.GetResources().GetFonts().font); // a font is required to make a text object
	pressEnterText.setCharacterSize(80);
	pressEnterText.setFillColor(sf::Color(33, 205, 52));

	pressEnterText.setOutlineColor(sf::Color(131, 246, 140));
	pressEnterText.setOutlineThickness(2.0f);

	sf::FloatRect pressEnterTextBounds = pressEnterText.getLocalBounds();

	sf::Vector2f pressEnterTextPosition = sf::Vector2f(
		ScreenLayout::SCREEN_WIDTH / 2 - pressEnterTextBounds.width / 2,
		ScreenLayout::SCREEN_HEIGHT / 2 - pressEnterTextBounds.height / 2 + 20 // offset height up by 200px
	);
	pressEnterText.setPosition(pressEnterTextPosition);



	// -- PRESS ESC TEXT --
	sf::Text pressEscText;

	pressEscText.setString("Esc to Quit");

	// Formatting
	pressEscText.setFont(mWorld.GetResources().GetFonts().font); // a font is required to make a text object
	pressEscText.setCharacterSize(80);
	pressEscText.setFillColor(sf::Color(225, 63, 63));

	pressEscText.setOutlineColor(sf::Color(255, 158, 158));
	pressEscText.setOutlineThickness(2.0f);

	sf::FloatRect pressEscTextBounds = pressEscText.getLocalBounds();

	sf::Vector2f pressEscTextPosition = sf::Vector2f(
		ScreenLayout::SCREEN_WIDTH / 2 - pressEscTextBounds.width / 2,
		ScreenLayout::SCREEN_HEIGHT / 2 - pressEscTextBounds.height / 2 + 130 // offset height up by 200px
	);
	pressEscText.setPosition(pressEscTextPosition);


	// -- RENDER --
	mWindow.GetRenderWindow().draw(titleText);
	mWindow.GetRenderWindow().draw(pressEnterText);
	mWindow.GetRenderWindow().draw(pressEscText);
}

void GameOverScreen::Shutdown() {
}
