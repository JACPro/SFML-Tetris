#include "ScoreManager.hpp"

#include "CONSTANTS_Gameplay.hpp"

bool InScoringRange(int value) {
	if (value >= Gameplay::MIN_LINES && value <= Gameplay::MAX_LINES) {
		return true;
	} else {
		return false;
	}
}

void ScoreManager::Notify(const int& value) {
	if (InScoringRange(value)) {
		// TODO this score should then be multiplied by current level
		score += Gameplay::SCORE_MULT[value-1];
	}
}
