#pragma once

#include "Observer.hpp"

class ScoreManager : public IObserver<int> {
public:
	explicit ScoreManager() : score(0) {}

	int GetScore() { return score; }

	void Notify(const int& value);


private:
	int score;

};