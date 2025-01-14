#include <SFML/Graphics.hpp>
#include <time.h>
#include <unordered_map>

#include "KeyHandler.hpp"

const int GRID_ROWS = 20;
const int GRID_COLS = 10;

const int SCREEN_WIDTH = 600;
const int SCREEN_HEIGHT = 800;

const int TILE_WIDTH = 30; // the width (pixels) of a single tile

const float DEFAULT_DELAY = 0.3f;

int field[GRID_ROWS][GRID_COLS];

sf::Vector2i currentTetromino[4], tempTetromino[4], nextTetromino[4];

// Next tetromino preview
const float nextTetrominoBoxPadding = 15.0f;
const sf::Vector2f nextTetrominoBoxPosition(400.0f, 100.0f);

 
int tetrominos[7][4] = {
	1, 3, 5, 7, // I
	2, 4, 5, 7, // Z
	3, 5, 4, 6, // S
	3, 5, 4, 7, // T
	2, 3, 5, 7, // L
	3, 5, 7, 6, // J
	2, 3, 4, 5, // O
};

bool check() {
	for (int i = 0; i < 4; i++) {
		if (currentTetromino[i].x < 0 || currentTetromino[i].x >= GRID_COLS || currentTetromino[i].y >= GRID_ROWS) {
			return 0;
		}
		else if (field[currentTetromino[i].y][currentTetromino[i].x] != -1) {
			return 0;
		}
	}

	return true;
}

int main() {
	srand(time(0));

	sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "SFML TETRIS");

	// Load Resources
	sf::Texture tiles, background, grid, frame, nextTileFrame;
	tiles.loadFromFile("Resources/Textures/Tiles.png");
	background.loadFromFile("Resources/Textures/Background.png");
	background.setRepeated(true);
	grid.loadFromFile("Resources/Textures/Grid.png");
	frame.loadFromFile("Resources/Textures/Frame.png");
	nextTileFrame.loadFromFile("Resources/Textures/NextTileFrame.png");
	sf::Font font;
	if (!font.loadFromFile("Resources/Fonts/Arial.ttf")) {
		return 0;
	}

	sf::Sprite tileSprite(tiles), backgroundSprite(background), gridSprite(grid), nextTileGridSprite(grid), 
		frameSprite(frame), nextTileFrameSprite(nextTileFrame);

	backgroundSprite.setTextureRect({0, 0, 600, 800});
	gridSprite.setPosition(50, 100);
	nextTileGridSprite.setTextureRect({ 0, 0, 90, 150});
	nextTileGridSprite.setPosition(nextTetrominoBoxPosition);
	nextTileFrameSprite.setPosition(nextTileGridSprite.getPosition().x -28, nextTileGridSprite.getPosition().y -28);
	
	// Initialize field 2D array
	for (int i = 0; i < GRID_ROWS; i++) {
		for (int j = 0; j < GRID_COLS; j++) {
			field[i][j] = -1;
		}
	}

	// Movement and gameplay
	int dx = 0;
	bool rotate = 0;
	int colourNum = 0, nextColourNum = 0;

	float timer = 0.0f;
	float delay = DEFAULT_DELAY;

	sf::Clock clock;

	// Generate random current tetromino
	int n = rand() % 7;
	colourNum = n;
	for (int i = 0; i < 4; i++) {
		currentTetromino[i].x = tetrominos[n][i] % 2;
		currentTetromino[i].y = tetrominos[n][i] / 2;
	}

	// Generate random next tetromino
	n = rand() % 7;
	nextColourNum = n;
	for (int i = 0; i < 4; i++) {
		nextTetromino[i].x = tetrominos[n][i] % 2;
		nextTetromino[i].y = tetrominos[n][i] / 2;
	}

	// Input 
	std::unordered_map<sf::Keyboard::Key, KeyHandler> keyHandlers;
	keyHandlers[sf::Keyboard::A] = KeyHandler(0.1f, [&]() { dx = -1; });
	keyHandlers[sf::Keyboard::D] = KeyHandler(0.1f, [&]() { dx = 1; });
	keyHandlers[sf::Keyboard::W] = KeyHandler(0.2f, [&]() { rotate = true; });
	keyHandlers[sf::Keyboard::S] = KeyHandler(0.0f, [&]() { delay = 0.05f; });

	while (window.isOpen()) {
		float deltaTime = clock.restart().asSeconds();
		timer += deltaTime;

		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				window.close();
			}
		}

		// Update key handlers
		for (auto& [key, handler] : keyHandlers) {
			handler.update(key, deltaTime);
		}

		// Move
		for (int i = 0; i < 4; i++) {
			tempTetromino[i] = currentTetromino[i];
			currentTetromino[i].x += dx;
		}

		if (!check()) {
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

			if (!check()) {
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

			if (!check()) {
				for (int i = 0; i < 4; i++) {
					field[tempTetromino[i].y][tempTetromino[i].x] = colourNum;
				}

				// Tetromino placed - fetch new 
				int n = rand() % 7;
				colourNum = nextColourNum;
				nextColourNum = n;
				for (int i = 0; i < 4; i++) {
					currentTetromino[i].x = nextTetromino[i].x;
					currentTetromino[i].y = nextTetromino[i].y;
					nextTetromino[i].x = tetrominos[n][i] % 2;
					nextTetromino[i].y = tetrominos[n][i] / 2;
				}

				if (!check()) {
					window.close();
				}
			}
			timer = 0;
		}

		// Check for lines
		int k = GRID_ROWS - 1;
		for (int i = GRID_ROWS - 1; i > 0; i--) {
			int count = 0;
			for (int j = 0; j < GRID_COLS; j++) {
				if (field[i][j] != -1) {
					count++;
				}
				field[k][j] = field[i][j];
			}
			if (count < GRID_COLS) {
				k--;
			}
		}

		dx = 0;
		rotate = false;
		delay = DEFAULT_DELAY;

		// Draw
		window.clear(sf::Color::White);
		window.draw(backgroundSprite);
		window.draw(gridSprite);

		// Grid
		for (int i = 0; i < GRID_ROWS; i++) {
			for (int j = 0; j < GRID_COLS; j++) {
				if (field[i][j] != -1) {
					tileSprite.setTextureRect(sf::IntRect(field[i][j] * TILE_WIDTH, 0, TILE_WIDTH, TILE_WIDTH));
					tileSprite.setPosition(j * TILE_WIDTH, i * TILE_WIDTH);
					tileSprite.move(50, 100);
					window.draw(tileSprite);
				}
			}
		}

		// Next Tetromino
		window.draw(nextTileGridSprite);
		window.draw(nextTileFrameSprite);

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

		float nextTetrominoBoxHorPadding = nextTileGridSprite.getGlobalBounds().width - 2 * TILE_WIDTH;
		nextTetrominoBoxHorPadding /= 2;

		float xOffset = nextTileGridSprite.getPosition().x + nextTetrominoBoxHorPadding;
		float yOffset = nextTileGridSprite.getPosition().y + nextTetrominoBoxPadding;
		if (!leftCol) { // I shape
			xOffset -= TILE_WIDTH / 2;
		}
		if (!topRow && bottomRow) { // No top row and not O shape = bottom 3 rows
			yOffset -= TILE_WIDTH / 2;
		}

		for (int i = 0; i < 4; i++) {
			tileSprite.setTextureRect(sf::IntRect(nextColourNum * TILE_WIDTH, 0, TILE_WIDTH, TILE_WIDTH));
			tileSprite.setPosition(
				nextTetromino[i].x * TILE_WIDTH + xOffset,
				nextTetromino[i].y * TILE_WIDTH + yOffset
			);			
			window.draw(tileSprite);
		}

		// Current Tetromino
		for (int i = 0; i < 4; i++) {
			tileSprite.setTextureRect(sf::IntRect(colourNum * TILE_WIDTH, 0, TILE_WIDTH, TILE_WIDTH));
			tileSprite.setPosition(currentTetromino[i].x * TILE_WIDTH, currentTetromino[i].y * TILE_WIDTH);
			tileSprite.move(50, 100);
			window.draw(tileSprite);
		}

		window.draw(frameSprite);
		window.display();
	}

	return 0;
}
