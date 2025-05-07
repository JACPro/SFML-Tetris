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
	, mScoreManager(mLevelManager)
{
	OnLinesCleared.AddObserver(&mScoreManager);
	OnLinesCleared.AddObserver(&mLevelManager);
	mLevelManager.OnLevelChanged.AddObserver(this);
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
	mNextTileGridEntity->GetSprite().setPosition(sf::Vector2f(ScreenLayout::RIGHT_UI_PANEL_POS_X, ScreenLayout::NEXT_TETROMINO_BOX_POS_Y));

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
	mKeyHandlers[sf::Keyboard::A] = KeyHandler(0.0f, [&]() { mDiffX = -1; });
	mKeyHandlers[sf::Keyboard::A].AssignNewKeyAction(EKeyboardEvents::Held, [&]() { mDiffX = -1; });
	mKeyHandlers[sf::Keyboard::A].SetFirstHeldRepeatDelay(0.167f);

	mKeyHandlers[sf::Keyboard::D] = KeyHandler(0.0f, [&]() { mDiffX = 1; });
	mKeyHandlers[sf::Keyboard::D].AssignNewKeyAction(EKeyboardEvents::Held, [&]() { mDiffX = 1; });
	mKeyHandlers[sf::Keyboard::D].SetFirstHeldRepeatDelay(0.167f);

	mKeyHandlers[sf::Keyboard::W] = KeyHandler(0.2f, [&]() { mRotate = true; });
	mKeyHandlers[sf::Keyboard::W].AssignNewKeyAction(EKeyboardEvents::Held, [&]() { mRotate = true; });

	mKeyHandlers[sf::Keyboard::S] = KeyHandler(0.0f, [&]() { mSoftDrop = true; });
	mKeyHandlers[sf::Keyboard::S].AssignNewKeyAction(EKeyboardEvents::Held, [&]() { mSoftDrop = true; });

	mKeyHandlers[sf::Keyboard::Space] = KeyHandler(0.0f, [&]() { HardDrop(); });

	mKeyHandlers[sf::Keyboard::Escape] = KeyHandler(0.1f, [&]() { Pause(); });
	mKeyHandlers[sf::Keyboard::Escape].mHandleWhilePaused = true;

	return true;
}


EScreens GameScreen::Update(float deltaTime) {
	mTimer += deltaTime;

	// Update key handlers
	for (auto& [key, handler] : mKeyHandlers) {
		if (!mIsPaused || handler.mHandleWhilePaused) {
			handler.Update(key, deltaTime);
		}
	}

	if (!mIsPaused) {
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
				if (mCurrentTetromino[0].x < 5) {
					// against left wall	
					int min = -1;
					for (int i = 0; i < 4; i++) {
						if (mCurrentTetromino[i].x < min) {
							min = mCurrentTetromino[i].x;
						}
					}

					for (int i = 0; i < 4; i++) {
						mCurrentTetromino[i].x -= min; // move right by minimum necessary amount - leftmost x value must be 0
					}
				} else {
					// against right wall
					int max = 10;
					for (int i = 0; i < 4; i++) {
						if (mCurrentTetromino[i].x > max) {
							max = mCurrentTetromino[i].x;
						}
					}

					for (int i = 0; i < 4; i++) {
						mCurrentTetromino[i].x -= (max - 9); // move left by minimum necessary amount - rightmost x value must be 9
					}
				}
			}
		}

		// Tick
		float moveDelay = mSoftDrop ? 0.05f : mDelay;
		if (mTimer > moveDelay) {
			for (int i = 0; i < 4; i++) {
				mTempTetromino[i] = mCurrentTetromino[i];
				mCurrentTetromino[i].y += 1;
			}

			if (!CheckIfLegalMove()) {
				PlaceTile();

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
			OnLinesCleared.NotifyObservers(clearedLines);
		}

		mDiffX = 0;
		mRotate = false;
		mSoftDrop = false;
	}

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
	float yOffset = nextTileGridSprite.getPosition().y + ScreenLayout::UI_BOX_PADDING;
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

	DrawCurrPieceToRenderTex();
	DrawDropGhostToRenderText();	

	return mNextScreen;
}

void GameScreen::Render() {
	RenderEntities();
	RenderCurrScoreBox();
	RenderCurrLevelBox();

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

void GameScreen::RenderCurrScoreBox() {
	// -- BACKGROUND --
	sf::RectangleShape scoreTextBox({ ScreenLayout::UI_BOX_WIDTH, ScreenLayout::SCORE_TEXT_BOX_HEIGHT });
	float nextTileFrameHeight = mNextTileFrameEntity->GetSprite().getTexture()->getSize().y;
	sf::Vector2f scoreTextBoxPosition = sf::Vector2f(
		ScreenLayout::RIGHT_UI_PANEL_POS_X,
		ScreenLayout::NEXT_TETROMINO_BOX_POS_Y + nextTileFrameHeight + ScreenLayout::SCORE_TEXT_BOX_PADDING
	);

	scoreTextBox.setPosition(scoreTextBoxPosition);
	scoreTextBox.setFillColor(ScreenLayout::UI_BOX_FILL_COLOUR);
	scoreTextBox.setOutlineColor(ScreenLayout::UI_BOX_OUTLINE_COLOUR);
	scoreTextBox.setOutlineThickness(ScreenLayout::UI_BOX_OUTLINE_THICKNESS);


	// -- TEXT --
	sf::Text scoreText;

	// Write text
	std::ostringstream stringBuff;
	stringBuff << "Score\n " << mScoreManager.GetScore();
	scoreText.setString(stringBuff.str());

	// Formatting
	scoreText.setFont(mWorld.GetResources().GetFonts().font); // a font is required to make a text object
	scoreText.setCharacterSize(36);
	scoreText.setFillColor(ScreenLayout::UI_BOX_TEXT_COLOUR);


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

void GameScreen::RenderCurrLevelBox() {
	// -- BACKGROUND --
	sf::RectangleShape levelTextBox({ ScreenLayout::UI_BOX_WIDTH, ScreenLayout::LEVEL_TEXT_BOX_HEIGHT });
	float nextTileFrameHeight = mNextTileFrameEntity->GetSprite().getTexture()->getSize().y;
	float scoreBoxHeightAndPadding = (
		ScreenLayout::SCORE_TEXT_BOX_HEIGHT + 
		ScreenLayout::UI_BOX_OUTLINE_THICKNESS * 2 + // outline of bottom of score box and outline of top of level box
		ScreenLayout::UI_BOX_PADDING 
	);
	sf::Vector2f levelTextBoxPosition = sf::Vector2f(
		ScreenLayout::RIGHT_UI_PANEL_POS_X,
		ScreenLayout::NEXT_TETROMINO_BOX_POS_Y + nextTileFrameHeight + ScreenLayout::LEVEL_TEXT_BOX_PADDING + scoreBoxHeightAndPadding
	);

	levelTextBox.setPosition(levelTextBoxPosition);
	levelTextBox.setFillColor(ScreenLayout::UI_BOX_FILL_COLOUR);
	levelTextBox.setOutlineColor(ScreenLayout::UI_BOX_OUTLINE_COLOUR);
	levelTextBox.setOutlineThickness(ScreenLayout::UI_BOX_OUTLINE_THICKNESS);


	// -- TEXT --
	sf::Text levelText;

	// Write text
	std::ostringstream stringBuff;
	stringBuff << "Level " << mLevelManager.GetLevel();
	levelText.setString(stringBuff.str());

	// Formatting
	levelText.setFont(mWorld.GetResources().GetFonts().font); // a font is required to make a text object
	levelText.setCharacterSize(36);
	levelText.setFillColor(ScreenLayout::UI_BOX_TEXT_COLOUR);


	sf::FloatRect scoreTextSize = levelText.getLocalBounds();

	sf::Vector2f levelTextPosition = sf::Vector2f(
		levelTextBoxPosition.x + ScreenLayout::LEVEL_TEXT_PADDING_X,
		levelTextBoxPosition.y + ScreenLayout::LEVEL_TEXT_PADDING_Y
	);
	levelText.setPosition(levelTextPosition);


	// -- RENDER --
	mWindow.GetRenderTex().draw(levelTextBox);
	mWindow.GetRenderTex().draw(levelText);
}

void GameScreen::DrawCurrPieceToRenderTex() {
	sf::Sprite& sprite = mTileEntity->GetSprite();

	for (int i = 0; i < 4; i++) {
		sprite.setTextureRect(sf::IntRect(
			mColourNum * ScreenLayout::TILE_WIDTH, 0,
			ScreenLayout::TILE_WIDTH, ScreenLayout::TILE_WIDTH)
		);
		sprite.setPosition(
			static_cast<float>(mCurrentTetromino[i].x * ScreenLayout::TILE_WIDTH),
			static_cast<float>(mCurrentTetromino[i].y * ScreenLayout::TILE_WIDTH)
		);
		sprite.move(50, 100);

		mTilesRenderTexture.draw(mTileEntity->GetSprite());
	}	
}

void GameScreen::DrawDropGhostToRenderText() {
	sf::Sprite& sprite = mTileEntity->GetSprite();

	// Save Initial Position
	for (int i = 0; i < 4; i++) {
		mTempTetromino[i] = mCurrentTetromino[i];;
	}

	// Move Down until colliding
	do {
		for (int i = 0; i < 4; i++) {
			mCurrentTetromino[i].y += 1;
		}
	} while (CheckIfLegalMove());

	// Move Up 1 to Last Legal Position
	for (int i = 0; i < 4; i++) {
		mCurrentTetromino[i].y -= 1;
	}

	// Render at Half Transparency
	sf::Color col = sprite.getColor();
	col.a = 150;
	sprite.setColor(col);

	// Current Piece now has Ghost piece properties
	DrawCurrPieceToRenderTex();

	// Restore Original Colour
	col.a = 255;
	sprite.setColor(col);

	// Restore Actual Position
	for (int i = 0; i < 4; i++) {
		mCurrentTetromino[i] = mTempTetromino[i];
	}
}

void GameScreen::PlaceTile() {
	// Set new placed tiles on grid
	for (int i = 0; i < 4; i++) {
		mGrid[mTempTetromino[i].y][mTempTetromino[i].x] = mColourNum;
	}

	// Fetch new Tetromino
	int n = GetNewTetrominoIndex();
	mColourNum = mNextColourNum;
	mNextColourNum = n;
	for (int i = 0; i < 4; i++) {
		mCurrentTetromino[i].x = mNextTetromino[i].x;
		mCurrentTetromino[i].y = mNextTetromino[i].y;
		mNextTetromino[i].x = Gameplay::TETROMINO_LAYOUTS[n][i] % 2;
		mNextTetromino[i].y = Gameplay::TETROMINO_LAYOUTS[n][i] / 2;
	}

	mTimer = 0;
}

void GameScreen::HardDrop() {
	// Move Down until colliding
	do {
		for (int i = 0; i < 4; i++) {
			mCurrentTetromino[i].y += 1;
		}
	} while (CheckIfLegalMove());

	// Move Up 1 to Last Legal Position
	for (int i = 0; i < 4; i++) {
		mCurrentTetromino[i].y -= 1;
	}

	// Set timer value hugh
	mTimer = 10.0f;
}

void GameScreen::Pause() {
	mIsPaused = !mIsPaused;
}

void GameScreen::Notify(const int& value) {
	SetNewFallDelayFromLevel(value);
}

