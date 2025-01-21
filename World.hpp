#pragma once

#include <unordered_map>

#include "Window.hpp"
#include "ResourceManager.hpp"
#include "KeyHandler.hpp"

#include "CONSTANTS_ScreenLayout.hpp"
#include "CONSTANTS_Gameplay.hpp"

class Entity;

class World {
public:
	explicit World(Window& window);

	void SetupSprites();
	bool Load();
	void Update(float deltaTime);
	void Render();
	void Shutdown();

	const ResourceManager& GetResources() const { return mResourceManager; }
	sf::RenderTexture& GetRenderTex() const { return mWindow.GetRenderTex(); }

private:
	// -- INFRASTRUCTURE FIELDS --
	Window& mWindow;
	ResourceManager mResourceManager;
	
	// -- GAMEPLAY FIELDS --
	int grid[ScreenLayout::GRID_ROWS][ScreenLayout::GRID_COLS];
	sf::Vector2i currentTetromino[4], tempTetromino[4], nextTetromino[4];

	Entity* TileEntity;
	Entity* NextTileGridEntity;

	int dx = 0;
	bool rotate = 0;
	int colourNum = 0, nextColourNum = 0;

	float timer = 0.0f;
	float delay = Gameplay::DEFAULT_DELAY;

	// Input
	std::unordered_map<sf::Keyboard::Key, KeyHandler> keyHandlers;

	// -- INFRASTRUCTURE FUNCTIONS --
	template<typename T>
	T* SpawnEntity(const sf::Texture& texture) {
		if (T* entity = new T(*this, texture)) {			
			mEntities.push_back(entity);
			return entity;
		} else {
			printf("ERROR: Failed to spawn entity\r\n");
			return nullptr;
		}
	}

	void RenderEntities();
	void DestroyAllEntities();

	std::vector<Entity*> mEntities;

	// -- GAMEPLAY FUNCTIONS --
	bool Check();
};