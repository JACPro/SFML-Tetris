#pragma once

namespace Gameplay {
	constexpr float DEFAULT_DELAY = 0.3f;

	constexpr int TETROMINO_LAYOUTS[7][4] = {
		1, 3, 5, 7, // I
		2, 4, 5, 7, // Z
		3, 5, 4, 6, // S
		3, 5, 4, 7, // T
		2, 3, 5, 7, // L
		3, 5, 7, 6, // J
		2, 3, 4, 5, // O
	};

	// Original Game Boy scoring
	constexpr int SCORE_MULT[] = { 
		40, // 1 line
		100, // 2 lines 
		300, // 3 lines
		1200 // 4 lines
	};

	// The min and max number of lines that can be scored in one go
	constexpr int MIN_LINES = 1;
	constexpr int MAX_LINES = sizeof(SCORE_MULT) / sizeof(SCORE_MULT[0]);
}
