#pragma once
#include <SFML/System/Vector2.hpp>

namespace ScreenLayout {
	constexpr int GRID_ROWS = 20;
	constexpr int GRID_COLS = 10;

	constexpr int SCREEN_WIDTH = 600;
	constexpr int SCREEN_HEIGHT = 800; 

	constexpr int TILE_WIDTH = 30; // the width (pixels) of a single tile

	// Next tetromino preview
	constexpr float NEXT_TETROMINO_BOX_PADDING = 15.0f;
	const sf::Vector2f NEXT_TETROMINO_BOX_POSITION(400.0f, 100.0f);

	// Score text box
	constexpr float SCORE_TEXT_BOX_PADDING = 15.0f; // gap from next tetromino preview above
	constexpr float SCORE_TEXT_PADDING_X = 10.0f; // away from edges
	constexpr float SCORE_TEXT_PADDING_Y = -5.0f; // away from edges
}
