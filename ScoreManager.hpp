#pragma once

#include "Observer.hpp"
#include "LevelManager.hpp"

class ScoreManager : public IObserver<int> {
public:
	explicit ScoreManager(LevelManager& levelManager);

	int GetScore() { return mScore; }

	void Notify(const int& value) override;


private:
	int mScore;

	LevelManager& mLevelManager;


	bool InScoringRange(int value);
};