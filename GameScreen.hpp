#pragma once
#include <unordered_map>

#include "Screen.hpp"
#include "Observable.hpp"
#include "CONSTANTS_ScreenLayout.hpp"
#include "CONSTANTS_Gameplay.hpp"
#include "ScoreManager.hpp"

#include "SFML/Graphics.hpp"

class Entity;
class Window;
class World;
class KeyHandler;


// TODO consider changing this inheritance from observable to a composition based structure
// where World has object of Observable type, because:
// 1. This would allow multiple events where we currently support only 1
// 2. Composition > Inheritance
class GameScreen : public IScreen, public Observable<int>{
public:
	explicit GameScreen(Window& window, World &world);

	void SetupSprites();
	
	bool Load() override;
	EScreens Update(float deltaTime) override;
    void Render() override;
	void Shutdown() override;

private:
	// -- INFRASTRUCTURE FIELDS --
	Window& mWindow;
	World& mWorld;

	std::vector<Entity*> mEntities;

	// -- GAMEPLAY FIELDS --
	int mGrid[ScreenLayout::GRID_ROWS][ScreenLayout::GRID_COLS];
	sf::Vector2i mCurrentTetromino[4], mTempTetromino[4], mNextTetromino[4];

	Entity* mTileEntity;
	Entity* mNextTileGridEntity;
	Entity* mNextTileFrameEntity;

	sf::RenderTexture mTilesRenderTexture;

	int mDiffX = 0;
	bool mRotate = false;
	int mColourNum = 0, mNextColourNum = 0;

	float mTimer = 0.0f;
	float mDelay = 1;

	std::vector<int> mTetrominoIndices{ 0, 1, 2, 3, 4, 5, 6 };
	int mNextTetrominoIndex = 7;

	// Input
	std::unordered_map<sf::Keyboard::Key, KeyHandler> mKeyHandlers;

	// TODO maybe scoreManager should belong to World
	ScoreManager mScoreManager;


	template<typename T>
	T* SpawnEntity(const sf::Texture& texture) {
		if (T* entity = new T(mWorld, texture)) {
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

	bool Check();

	int GetNewTetrominoIndex();

	void SetNewFallDelayFromLevel(int level);
};