#include "GameScreen.hpp"

#include <sstream>
#include "KeyHandler.hpp"
#include "Entity.hpp"

#include "World.hpp"
#include "Window.hpp"
#include <random>


GameScreen::GameScreen(Window& window, World& world)
	: mWindow(window)
	, mWorld(world)
{
	AddObserver(&mScoreManager);
	mTilesRenderTexture.create(ScreenLayout::SCREEN_WIDTH, ScreenLayout::SCREEN_HEIGHT);
}

void GameScreen::SetupSprites() {
	const ResourceManager& resourceManager = mWorld.GetResources();
	mTileEntity = SpawnEntity<Entity>(resourceManager.GetTextures().tiles);

	Entity* BackgroundEntity = SpawnEntity<Entity>(resourceManager.GetTextures().background);
	BackgroundEntity->GetSprite().setTextureRect({ 0, 0, ScreenLayout::SCREEN_WIDTH, ScreenLayout::SCREEN_HEIGHT });

	Entity* GridEntity = SpawnEntity<Entity>(resourceManager.GetTextures().grid);
	GridEntity->GetSprite().setPosition(50, 100);

	mNextTileGridEntity = SpawnEntity<Entity>(resourceManager.GetTextures().grid);
	mNextTileGridEntity->GetSprite().setTextureRect({ 0, 0, 90, 150 });
	mNextTileGridEntity->GetSprite().setPosition(ScreenLayout::NEXT_TETROMINO_BOX_POSITION);

	Entity* FrameEntity = SpawnEntity<Entity>(resourceManager.GetTextures().frame);

	sf::Vector2f nextTileBgPos = mNextTileGridEntity->GetSprite().getPosition();
	mNextTileFrameEntity = SpawnEntity<Entity>(resourceManager.GetTextures().nextTileFrame);
	mNextTileFrameEntity->GetSprite().setPosition(nextTileBgPos.x - 28, nextTileBgPos.y - 28);
}

bool GameScreen::Load() {

	SetupSprites();


	// Initialize field 2D array
	for (int i = 0; i < ScreenLayout::GRID_ROWS; i++) {
		for (int j = 0; j < ScreenLayout::GRID_COLS; j++) {
			mGrid[i][j] = -1;
		}
	}

	// Generate random first tetromino
	int n = GetNewTetrominoIndex();
	mColourNum = n;
	for (int i = 0; i < 4; i++) {
		mCurrentTetromino[i].x = Gameplay::TETROMINO_LAYOUTS[n][i] % 2;
		mCurrentTetromino[i].y = Gameplay::TETROMINO_LAYOUTS[n][i] / 2;
	}

	// Generate random next tetromino
	n = GetNewTetrominoIndex();
	mNextColourNum = n;
	for (int i = 0; i < 4; i++) {
		mNextTetromino[i].x = Gameplay::TETROMINO_LAYOUTS[n][i] % 2;
		mNextTetromino[i].y = Gameplay::TETROMINO_LAYOUTS[n][i] / 2;
	}

	// Input 
	mKeyHandlers[sf::Keyboard::A] = KeyHandler(0.1f, [&]() { mDiffX = -1; });
	mKeyHandlers[sf::Keyboard::A].AssignNewKeyAction(EKeyboardEvents::Held, [&]() { mDiffX = -1; });

	mKeyHandlers[sf::Keyboard::D] = KeyHandler(0.1f, [&]() { mDiffX = 1; });
	mKeyHandlers[sf::Keyboard::D].AssignNewKeyAction(EKeyboardEvents::Held, [&]() { mDiffX = 1; });

	mKeyHandlers[sf::Keyboard::W] = KeyHandler(0.2f, [&]() { mRotate = true; });
	mKeyHandlers[sf::Keyboard::W].AssignNewKeyAction(EKeyboardEvents::Held, [&]() { mRotate = true; });

	mKeyHandlers[sf::Keyboard::S] = KeyHandler(0.0f, [&]() { mDelay = 0.05f; });
	mKeyHandlers[sf::Keyboard::S].AssignNewKeyAction(EKeyboardEvents::Held, [&]() { mDelay = 0.05f; });


	return true;
}


EScreens GameScreen::Update(float deltaTime) {
	mTimer += deltaTime;

	// Update key handlers
	for (auto& [key, handler] : mKeyHandlers) {
		handler.Update(key, deltaTime);
	}

	// Move
	for (int i = 0; i < 4; i++) {
		mTempTetromino[i] = mCurrentTetromino[i];
		mCurrentTetromino[i].x += mDiffX;
	}

	if (!CheckIfLegalMove()) {
		for (int i = 0; i < 4; i++) {
			mCurrentTetromino[i] = mTempTetromino[i];
		}
	}

	// Rotate
	if (mRotate) {
		sf::Vector2i point = mCurrentTetromino[1];
		for (int i = 0; i < 4; i++) {
			int x = mCurrentTetromino[i].y - point.y;
			int y = mCurrentTetromino[i].x - point.x;
			mCurrentTetromino[i].x = point.x - x;
			mCurrentTetromino[i].y = point.y + y;
		}

		if (!CheckIfLegalMove()) {
			for (int i = 0; i < 4; i++) {
				mCurrentTetromino[i] = mTempTetromino[i];
			}
		}
	}

	// Tick
	if (mTimer > mDelay) {
		for (int i = 0; i < 4; i++) {
			mTempTetromino[i] = mCurrentTetromino[i];
			mCurrentTetromino[i].y += 1;
		}

		if (!CheckIfLegalMove()) {
			for (int i = 0; i < 4; i++) {
				mGrid[mTempTetromino[i].y][mTempTetromino[i].x] = mColourNum;
			}

			// Tetromino placed - fetch new 
			int n = GetNewTetrominoIndex();
			mColourNum = mNextColourNum;
			mNextColourNum = n;
			for (int i = 0; i < 4; i++) {
				mCurrentTetromino[i].x = mNextTetromino[i].x;
				mCurrentTetromino[i].y = mNextTetromino[i].y;
				mNextTetromino[i].x = Gameplay::TETROMINO_LAYOUTS[n][i] % 2;
				mNextTetromino[i].y = Gameplay::TETROMINO_LAYOUTS[n][i] / 2;
			}

			if (!CheckIfLegalMove()) {
				return EScreens::GameOver;
			}
		}
		mTimer = 0;
	}

	// Check for lines
	int k = ScreenLayout::GRID_ROWS - 1;
	int clearedLines = 0;
	for (int i = ScreenLayout::GRID_ROWS - 1; i > 0; i--) {
		int count = 0;
		for (int j = 0; j < ScreenLayout::GRID_COLS; j++) {
			if (mGrid[i][j] != -1) {
				count++;
			}
			mGrid[k][j] = mGrid[i][j];
		}
		if (count < ScreenLayout::GRID_COLS) {
			k--;
		} else {
			clearedLines++;
		}
	}
	if (clearedLines) {
		NotifyObservers(clearedLines);
	}

	mDiffX = 0;
	mRotate = false;
	mDelay = Gameplay::DEFAULT_DELAY;

	// Grid
	for (int i = 0; i < ScreenLayout::GRID_ROWS; i++) {
		for (int j = 0; j < ScreenLayout::GRID_COLS; j++) {
			if (mGrid[i][j] != -1) {
				mTileEntity->GetSprite().setTextureRect(sf::IntRect(
					mGrid[i][j] * ScreenLayout::TILE_WIDTH, 0,
					ScreenLayout::TILE_WIDTH, ScreenLayout::TILE_WIDTH)
				);
				mTileEntity->GetSprite().setPosition(
					static_cast<float>(j * ScreenLayout::TILE_WIDTH),
					static_cast<float>(i * ScreenLayout::TILE_WIDTH)
				);
				mTileEntity->GetSprite().move(50, 100);
				mTilesRenderTexture.draw(mTileEntity->GetSprite());
			}
		}
	}

	// Next Tetromino
	// If no tile square is in first row or col, need offset to center
	bool topRow = false, leftCol = false, bottomRow = false;
	for (int i = 0; i < 4; i++) {
		if (mNextTetromino[i].x == 0) {
			leftCol = true; // there is a square in the left col
		}
		if (mNextTetromino[i].y == 0) {
			topRow = true; // there is a square in the top row
		}
		if (mNextTetromino[i].y == 3) {
			bottomRow = true; // there is a square in the bottom row
		}
	}

	const sf::Sprite nextTileGridSprite = mNextTileGridEntity->GetSprite();
	float nextTetrominoBoxHorPadding = nextTileGridSprite.getGlobalBounds().width - 2 * ScreenLayout::TILE_WIDTH;
	nextTetrominoBoxHorPadding /= 2;

	float xOffset = nextTileGridSprite.getPosition().x + nextTetrominoBoxHorPadding;
	float yOffset = nextTileGridSprite.getPosition().y + ScreenLayout::NEXT_TETROMINO_BOX_PADDING;
	if (!leftCol) { // I shape
		xOffset -= ScreenLayout::TILE_WIDTH / 2;
	}
	if (!topRow && bottomRow) { // No top row and not O shape = bottom 3 rows
		yOffset -= ScreenLayout::TILE_WIDTH / 2;
	}

	for (int i = 0; i < 4; i++) {
		mTileEntity->GetSprite().setTextureRect(sf::IntRect(
			mNextColourNum * ScreenLayout::TILE_WIDTH, 0,
			ScreenLayout::TILE_WIDTH, ScreenLayout::TILE_WIDTH)
		);
		mTileEntity->GetSprite().setPosition(
			mNextTetromino[i].x * ScreenLayout::TILE_WIDTH + xOffset,
			mNextTetromino[i].y * ScreenLayout::TILE_WIDTH + yOffset
		);
		mTilesRenderTexture.draw(mTileEntity->GetSprite());
	}

	// Current Tetromino
	for (int i = 0; i < 4; i++) {
		mTileEntity->GetSprite().setTextureRect(sf::IntRect(
			mColourNum * ScreenLayout::TILE_WIDTH, 0,
			ScreenLayout::TILE_WIDTH, ScreenLayout::TILE_WIDTH)
		);
		mTileEntity->GetSprite().setPosition(
			static_cast<float>(mCurrentTetromino[i].x * ScreenLayout::TILE_WIDTH),
			static_cast<float>(mCurrentTetromino[i].y * ScreenLayout::TILE_WIDTH)
		);

		mTileEntity->GetSprite().move(50, 100);
		mTilesRenderTexture.draw(mTileEntity->GetSprite());
	}

	return mNextScreen;
}

void GameScreen::Render() {
	RenderEntities();
	RenderScore();

	sf::Sprite fullScreenSprite(mWorld.GetRenderTex().getTexture());

	fullScreenSprite.setScale(
		mWindow.GetRenderWindow().getSize().x / static_cast<float>(mWorld.GetRenderTex().getSize().x),
		mWindow.GetRenderWindow().getSize().y / static_cast<float>(mWorld.GetRenderTex().getSize().y)
	);

	const sf::Vector2f& viewSize = mWindow.GetRenderWindow().getView().getSize();
	fullScreenSprite.setOrigin(sf::Vector2f(0, 0));

	mWindow.GetRenderWindow().draw(fullScreenSprite);

	mTilesRenderTexture.display();
	sf::Sprite TilesSprite(mTilesRenderTexture.getTexture());
	mWindow.GetRenderWindow().draw(TilesSprite);
	mTilesRenderTexture.clear(sf::Color(0, 0, 0, 0));

	mWorld.GetRenderTex().display();
}

void GameScreen::Shutdown() {
	DestroyAllEntities();
}

void GameScreen::RenderEntities() {
	int count = 0;
	for (Entity* entity : mEntities) {
		if (entity != nullptr) {
			entity->Render(mWorld.GetRenderTex());
		}
	}
}

void GameScreen::DestroyAllEntities() {
	for (Entity* entity : mEntities) {
		delete entity;
	}

	mEntities.clear();
}

bool GameScreen::CheckIfLegalMove() {
	for (int i = 0; i < 4; i++) {
		if (mCurrentTetromino[i].x < 0 || // too far left
			mCurrentTetromino[i].x >= ScreenLayout::GRID_COLS || // too far right
			mCurrentTetromino[i].y >= ScreenLayout::GRID_ROWS) // too far down
		{
			return 0;
		} else if (mGrid[mCurrentTetromino[i].y][mCurrentTetromino[i].x] != -1) {
			return 0;
		}
	}

	return true;
}

int GameScreen::GetNewTetrominoIndex() {
	if (mNextTetrominoIndex >= mTetrominoIndices.size()) {
		std::random_device rd;
		std::mt19937 rng(rd());
		std::shuffle(mTetrominoIndices.begin(), mTetrominoIndices.end(), rng);
		mNextTetrominoIndex = 0;
	}
	
	return mTetrominoIndices[mNextTetrominoIndex++];
}

void GameScreen::SetNewFallDelayFromLevel(int level) {
	// speed curve as defined in Tetris Worlds (https://tetris.wiki/Marathon)
	mDelay = std::pow((0.8 - ((level - 1) * 0.007)), (level - 1));
}

void GameScreen::RenderScore() {
	// -- BACKGROUND --
	sf::RectangleShape scoreTextBox({ 190.f, 80.f });
	float nextTileFrameHeight = mNextTileFrameEntity->GetSprite().getTexture()->getSize().y;
	sf::Vector2f scoreTextBoxPosition = sf::Vector2f(
		ScreenLayout::NEXT_TETROMINO_BOX_POSITION.x,
		ScreenLayout::NEXT_TETROMINO_BOX_POSITION.y + nextTileFrameHeight + ScreenLayout::SCORE_TEXT_BOX_PADDING
	);

	scoreTextBox.setPosition(scoreTextBoxPosition);
	scoreTextBox.setFillColor(sf::Color(34, 88, 130));
	scoreTextBox.setOutlineColor(sf::Color(175, 227, 254));
	scoreTextBox.setOutlineThickness(10.0f);


	// -- TEXT --
	sf::Text scoreText;

	// Write text
	std::ostringstream stringBuff;
	stringBuff << "Score\n " << mScoreManager.GetScore();
	scoreText.setString(stringBuff.str());

	// Formatting
	scoreText.setFont(mWorld.GetResources().GetFonts().font); // a font is required to make a text object
	scoreText.setCharacterSize(36);
	scoreText.setFillColor(sf::Color(175, 227, 254));


	sf::FloatRect scoreTextSize = scoreText.getLocalBounds();

	sf::Vector2f scoreTextPosition = sf::Vector2f(
		scoreTextBoxPosition.x + ScreenLayout::SCORE_TEXT_PADDING_X,
		scoreTextBoxPosition.y + ScreenLayout::SCORE_TEXT_PADDING_Y
	);
	scoreText.setPosition(scoreTextPosition);


	// -- RENDER --
	mWindow.GetRenderTex().draw(scoreTextBox);
	mWindow.GetRenderTex().draw(scoreText);
}
