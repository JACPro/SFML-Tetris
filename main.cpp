#include <SFML/Graphics.hpp>
#include <time.h>
#include <unordered_map>

#include "KeyHandler.hpp"

const int GRID_ROWS = 20;
const int GRID_COLS = 10;

const int SCREEN_WIDTH = 320;
const int SCREEN_HEIGHT = 480;

const int TILE_WIDTH = 18; // the width (pixels) of a single tile

const float DEFAULT_DELAY = 0.3f;

int field[GRID_ROWS][GRID_COLS] = { 0 };

sf::Vector2i currentTetromino[4], tempTetromino[4];

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
		else if (field[currentTetromino[i].y][currentTetromino[i].x]) {
			return 0;
		}
	}

	return true;
}

int main() {
	srand(time(0));

	sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "SFML TETRIS");

	sf::Texture tiles, background, frame;
	tiles.loadFromFile("Resources/Textures/tiles.png");
	background.loadFromFile("Resources/Textures/background.png");
	frame.loadFromFile("Resources/Textures/frame.png");

	sf::Sprite tileSprite(tiles), backgroundSprite(background), frameSprite(frame);

	int dx = 0;
	bool rotate = 0;
	int colourNum = 1;

	float timer = 0.0f;
	float delay = DEFAULT_DELAY;

	sf::Clock clock;

	int n = rand() % 7;
	colourNum = 1 + n;
	for (int i = 0; i < 4; i++) {
		currentTetromino[i].x = tetrominos[n][i] % 2;
		currentTetromino[i].y = tetrominos[n][i] / 2;
	}


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

				int n = rand() % 7;
				colourNum = 1 + n;
				// Tetromino placed - fetch new 
				for (int i = 0; i < 4; i++) {
					currentTetromino[i].x = tetrominos[n][i] % 2;
					currentTetromino[i].y = tetrominos[n][i] / 2;
				}
			}
			timer = 0;
		}

		// Check for lines
		int k = GRID_ROWS - 1;
		for (int i = GRID_ROWS - 1; i > 0; i--) {
			int count = 0;
			for (int j = 0; j < GRID_COLS; j++) {
				if (field[i][j]) {
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

		for (int i = 0; i < GRID_ROWS; i++) {
			for (int j = 0; j < GRID_COLS; j++) {
				if (field[i][j] != 0) {
					tileSprite.setTextureRect(sf::IntRect(field[i][j] * TILE_WIDTH, 0, TILE_WIDTH, TILE_WIDTH));
					tileSprite.setPosition(j * TILE_WIDTH, i * TILE_WIDTH);
					tileSprite.move(28, 31);
					window.draw(tileSprite);
				}
			}
		}

		for (int i = 0; i < 4; i++) {
			tileSprite.setTextureRect(sf::IntRect(colourNum * TILE_WIDTH, 0, TILE_WIDTH, TILE_WIDTH));
			tileSprite.setPosition(currentTetromino[i].x * TILE_WIDTH, currentTetromino[i].y * TILE_WIDTH);
			tileSprite.move(28, 31);
			window.draw(tileSprite);
		}

		window.draw(frameSprite);
		window.display();
	}

	return 0;
}