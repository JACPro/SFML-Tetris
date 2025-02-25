#pragma once

#include "Observer.hpp"
#include "Observable.hpp"

class LevelManager : public IObserver<int> {
public:
	explicit LevelManager();

	int GetLevel() { return mCurrentLevel; }

	void Notify(const int& value) override;

	// Events
	Observable<int> OnLevelChanged = Observable<int>();

private:
	int mCurrentLevel;
	int mLinesThisLevel;
};
