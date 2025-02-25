#pragma once

#include <unordered_map>
#include <SFML/Graphics.hpp>

#include "Screen.hpp"

class Window;
class World;
class KeyHandler;

class GameOverScreen : public IScreen {
public:
	explicit GameOverScreen(Window& window, World& world);

	void SetupSprites();

	bool Load() override;
	EScreens Update(float deltaTime) override;
	void Render() override;
	void Shutdown() override;

private:
	Window& mWindow;
	World& mWorld;

	// Input
	std::unordered_map<sf::Keyboard::Key, KeyHandler> keyHandlers;

	sf::Sprite mBackgroundSprites[4];
	sf::Vector2f mBackgroundScrollSpeed = { 20.0f, 20.0f };
	sf::Vector2i mBackgroundTileSize;
};