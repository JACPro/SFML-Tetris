#include "GameScreen.hpp"

#include <sstream>
#include "KeyHandler.hpp"
#include "Entity.hpp"

#include "World.hpp"
#include "Window.hpp"


GameScreen::GameScreen(Window& window, World& world)
	: mWindow(window)
	, mWorld(world)
{
	AddObserver(&scoreManager);
	TilesRenderTexture.create(ScreenLayout::SCREEN_WIDTH, ScreenLayout::SCREEN_HEIGHT);
}

void GameScreen::SetupSprites() {
	const ResourceManager& resourceManager = mWorld.GetResources();
	TileEntity = SpawnEntity<Entity>(resourceManager.GetTextures().tiles);

	Entity* BackgroundEntity = SpawnEntity<Entity>(resourceManager.GetTextures().background);
	BackgroundEntity->GetSprite().setTextureRect({ 0, 0, ScreenLayout::SCREEN_WIDTH, ScreenLayout::SCREEN_HEIGHT });

	Entity* GridEntity = SpawnEntity<Entity>(resourceManager.GetTextures().grid);
	GridEntity->GetSprite().setPosition(50, 100);

	NextTileGridEntity = SpawnEntity<Entity>(resourceManager.GetTextures().grid);
	NextTileGridEntity->GetSprite().setTextureRect({ 0, 0, 90, 150 });
	NextTileGridEntity->GetSprite().setPosition(ScreenLayout::NEXT_TETROMINO_BOX_POSITION);

	Entity* FrameEntity = SpawnEntity<Entity>(resourceManager.GetTextures().frame);

	sf::Vector2f nextTileBgPos = NextTileGridEntity->GetSprite().getPosition();
	NextTileFrameEntity = SpawnEntity<Entity>(resourceManager.GetTextures().nextTileFrame);
	NextTileFrameEntity->GetSprite().setPosition(nextTileBgPos.x - 28, nextTileBgPos.y - 28);
}

bool GameScreen::Load() {

	SetupSprites();


	// Initialize field 2D array
	for (int i = 0; i < ScreenLayout::GRID_ROWS; i++) {
		for (int j = 0; j < ScreenLayout::GRID_COLS; j++) {
			grid[i][j] = -1;
		}
	}

	// Generate random first tetromino
	int n = rand() % 7;
	colourNum = n;
	for (int i = 0; i < 4; i++) {
		currentTetromino[i].x = Gameplay::TETROMINO_LAYOUTS[n][i] % 2;
		currentTetromino[i].y = Gameplay::TETROMINO_LAYOUTS[n][i] / 2;
	}

	// Generate random next tetromino
	n = rand() % 7;
	nextColourNum = n;
	for (int i = 0; i < 4; i++) {
		nextTetromino[i].x = Gameplay::TETROMINO_LAYOUTS[n][i] % 2;
		nextTetromino[i].y = Gameplay::TETROMINO_LAYOUTS[n][i] / 2;
	}

	// Input 
	keyHandlers[sf::Keyboard::A] = KeyHandler(0.1f, [&]() { dx = -1; });
	keyHandlers[sf::Keyboard::D] = KeyHandler(0.1f, [&]() { dx = 1; });
	keyHandlers[sf::Keyboard::W] = KeyHandler(0.2f, [&]() { rotate = true; });
	keyHandlers[sf::Keyboard::S] = KeyHandler(0.0f, [&]() { delay = 0.05f; });

	return true;
}


EScreens GameScreen::Update(float deltaTime) {
	timer += deltaTime;

	// Update key handlers
	for (auto& [key, handler] : keyHandlers) {
		handler.update(key, deltaTime);
	}

	// Move
	for (int i = 0; i < 4; i++) {
		tempTetromino[i] = currentTetromino[i];
		currentTetromino[i].x += dx;
	}

	if (!Check()) {
		for (int i = 0; i < 4; i++) {
			currentTetromino[i] = tempTetromino[i];
		}
	}

	// Rotate
	if (rotate) {
		sf::Vector2i point = currentTetromino[1];
		for (int i = 0; i < 4; i++) {
			int x = currentTetromino[i].y - point.y;
			int y = currentTetromino[i].x - point.x;
			currentTetromino[i].x = point.x - x;
			currentTetromino[i].y = point.y + y;
		}

		if (!Check()) {
			for (int i = 0; i < 4; i++) {
				currentTetromino[i] = tempTetromino[i];
			}
		}
	}

	// Tick
	if (timer > delay) {
		for (int i = 0; i < 4; i++) {
			tempTetromino[i] = currentTetromino[i];
			currentTetromino[i].y += 1;
		}

		if (!Check()) {
			for (int i = 0; i < 4; i++) {
				grid[tempTetromino[i].y][tempTetromino[i].x] = colourNum;
			}

			// Tetromino placed - fetch new 
			int n = rand() % 7;
			colourNum = nextColourNum;
			nextColourNum = n;
			for (int i = 0; i < 4; i++) {
				currentTetromino[i].x = nextTetromino[i].x;
				currentTetromino[i].y = nextTetromino[i].y;
				nextTetromino[i].x = Gameplay::TETROMINO_LAYOUTS[n][i] % 2;
				nextTetromino[i].y = Gameplay::TETROMINO_LAYOUTS[n][i] / 2;
			}

			if (!Check()) {
				return EScreens::GameOver;
			}
		}
		timer = 0;
	}

	// Check for lines
	int k = ScreenLayout::GRID_ROWS - 1;
	int clearedLines = 0;
	for (int i = ScreenLayout::GRID_ROWS - 1; i > 0; i--) {
		int count = 0;
		for (int j = 0; j < ScreenLayout::GRID_COLS; j++) {
			if (grid[i][j] != -1) {
				count++;
			}
			grid[k][j] = grid[i][j];
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


	dx = 0;
	rotate = false;
	delay = Gameplay::DEFAULT_DELAY;


	// Grid
	for (int i = 0; i < ScreenLayout::GRID_ROWS; i++) {
		for (int j = 0; j < ScreenLayout::GRID_COLS; j++) {
			if (grid[i][j] != -1) {
				TileEntity->GetSprite().setTextureRect(sf::IntRect(
					grid[i][j] * ScreenLayout::TILE_WIDTH, 0,
					ScreenLayout::TILE_WIDTH, ScreenLayout::TILE_WIDTH)
				);
				TileEntity->GetSprite().setPosition(
					static_cast<float>(j * ScreenLayout::TILE_WIDTH),
					static_cast<float>(i * ScreenLayout::TILE_WIDTH)
				);
				TileEntity->GetSprite().move(50, 100);
				TilesRenderTexture.draw(TileEntity->GetSprite());
			}
		}
	}

	// Next Tetromino
	// If no tile square is in first row or col, need offset to center
	bool topRow = false, leftCol = false, bottomRow = false;
	for (int i = 0; i < 4; i++) {
		if (nextTetromino[i].x == 0) {
			leftCol = true; // there is a square in the left col
		}
		if (nextTetromino[i].y == 0) {
			topRow = true; // there is a square in the top row
		}
		if (nextTetromino[i].y == 3) {
			bottomRow = true; // there is a square in the bottom row
		}
	}

	const sf::Sprite nextTileGridSprite = NextTileGridEntity->GetSprite();
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
		TileEntity->GetSprite().setTextureRect(sf::IntRect(
			nextColourNum * ScreenLayout::TILE_WIDTH, 0,
			ScreenLayout::TILE_WIDTH, ScreenLayout::TILE_WIDTH)
		);
		TileEntity->GetSprite().setPosition(
			nextTetromino[i].x * ScreenLayout::TILE_WIDTH + xOffset,
			nextTetromino[i].y * ScreenLayout::TILE_WIDTH + yOffset
		);
		TilesRenderTexture.draw(TileEntity->GetSprite());
	}

	// Current Tetromino
	for (int i = 0; i < 4; i++) {
		TileEntity->GetSprite().setTextureRect(sf::IntRect(
			colourNum * ScreenLayout::TILE_WIDTH, 0,
			ScreenLayout::TILE_WIDTH, ScreenLayout::TILE_WIDTH)
		);
		TileEntity->GetSprite().setPosition(
			static_cast<float>(currentTetromino[i].x * ScreenLayout::TILE_WIDTH),
			static_cast<float>(currentTetromino[i].y * ScreenLayout::TILE_WIDTH)
		);

		TileEntity->GetSprite().move(50, 100);
		TilesRenderTexture.draw(TileEntity->GetSprite());
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

	TilesRenderTexture.display();
	sf::Sprite TilesSprite(TilesRenderTexture.getTexture());
	mWindow.GetRenderWindow().draw(TilesSprite);
	TilesRenderTexture.clear(sf::Color(0, 0, 0, 0));

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

bool GameScreen::Check() {
	for (int i = 0; i < 4; i++) {
		if (currentTetromino[i].x < 0 || // too far left
			currentTetromino[i].x >= ScreenLayout::GRID_COLS || // too far right
			currentTetromino[i].y >= ScreenLayout::GRID_ROWS) // too far down
		{
			return 0;
		} else if (grid[currentTetromino[i].y][currentTetromino[i].x] != -1) {
			return 0;
		}
	}

	return true;
}

void GameScreen::RenderScore() {
	// -- BACKGROUND --
	sf::RectangleShape scoreTextBox({ 190.f, 80.f });
	float nextTileFrameHeight = NextTileFrameEntity->GetSprite().getTexture()->getSize().y;
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
	stringBuff << "Score\n " << scoreManager.GetScore();
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
