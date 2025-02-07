#pragma once

#include <unordered_map>

#include "Window.hpp"
#include "ResourceManager.hpp"
#include "KeyHandler.hpp"

#include "CONSTANTS_ScreenLayout.hpp"
#include "CONSTANTS_Gameplay.hpp"

#include "Observable.hpp"
#include "ScoreManager.hpp"

class Entity;

// TODO consider changing this inheritance from observable to a composition based structure
// where World has object of Observable type, because:
// 1. This would allow multiple events where we currently support only 1
// 2. Composition > Inheritance
class World : public Observable<int> {
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
	
	std::vector<Entity*> mEntities;

	// -- GAMEPLAY FIELDS --
	int grid[ScreenLayout::GRID_ROWS][ScreenLayout::GRID_COLS];
	sf::Vector2i currentTetromino[4], tempTetromino[4], nextTetromino[4];

	Entity* TileEntity;
	Entity* NextTileGridEntity;
	Entity* NextTileFrameEntity;

	int dx = 0;
	bool rotate = 0;
	int colourNum = 0, nextColourNum = 0;

	float timer = 0.0f;
	float delay = Gameplay::DEFAULT_DELAY;

	// Input
	std::unordered_map<sf::Keyboard::Key, KeyHandler> keyHandlers;

	ScoreManager scoreManager;


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

	void RenderScore();

	// -- GAMEPLAY FUNCTIONS --
	bool Check();
};
