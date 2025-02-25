#include "ScoreManager.hpp"

#include "CONSTANTS_Gameplay.hpp"

ScoreManager::ScoreManager(LevelManager& levelManager)
	: mScore(0)
	, mLevelManager(levelManager)
{
}

bool ScoreManager::InScoringRange(int value) {
	if (value >= Gameplay::MIN_LINES && value <= Gameplay::MAX_LINES) {
		return true;
	} else {
		return false;
	}
}

void ScoreManager::Notify(const int& value) {
	if (InScoringRange(value)) {
		mScore += (Gameplay::SCORE_MULT[value-1] * mLevelManager.GetLevel());
	}
}
