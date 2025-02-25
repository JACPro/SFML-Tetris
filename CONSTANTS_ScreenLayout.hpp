#pragma once
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Color.hpp>

namespace ScreenLayout {
	constexpr int GRID_ROWS = 20;
	constexpr int GRID_COLS = 10;

	constexpr int SCREEN_WIDTH = 600;
	constexpr int SCREEN_HEIGHT = 800; 

	constexpr int TILE_WIDTH = 30; // the width (pixels) of a single tile


	// UI General
	constexpr float RIGHT_UI_PANEL_POS_X = 400.0f;

	constexpr float UI_BOX_WIDTH = 190.0f;
	constexpr float UI_BOX_PADDING = 15.0f;
	constexpr float UI_BOX_OUTLINE_THICKNESS = 10.0f;
	const sf::Color UI_BOX_FILL_COLOUR = sf::Color(34, 88, 130);
	const sf::Color UI_BOX_OUTLINE_COLOUR = sf::Color(175, 227, 254);
	const sf::Color UI_BOX_TEXT_COLOUR = UI_BOX_OUTLINE_COLOUR;

	// Next Tetromino Box
	constexpr float NEXT_TETROMINO_BOX_POS_Y = 100.0f;

	// Score text box
	constexpr float SCORE_TEXT_BOX_PADDING = 15.0f; // gap from next tetromino preview above
	constexpr float SCORE_TEXT_BOX_HEIGHT = 80.0f;
	constexpr float SCORE_TEXT_PADDING_X = 10.0f; // away from edges
	constexpr float SCORE_TEXT_PADDING_Y = -5.0f; // away from edges

	// Level text box
	constexpr float LEVEL_TEXT_BOX_PADDING = 15.0f; // gap from next tetromino preview above
	constexpr float LEVEL_TEXT_BOX_HEIGHT = 50.0f;
	constexpr float LEVEL_TEXT_PADDING_X = 10.0f; // away from edges
	constexpr float LEVEL_TEXT_PADDING_Y = -5.0f; // away from edges
}
