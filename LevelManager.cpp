#include "LevelManager.hpp"

LevelManager::LevelManager() 
	: mCurrentLevel(1) 
	, mLinesThisLevel(0) 
{
}

void LevelManager::Notify(const int& value) {
	mLinesThisLevel += value;

	if (mLinesThisLevel >= 10) {
		mCurrentLevel++;
		mLinesThisLevel %= 10;
		OnLevelChanged.NotifyObservers(mCurrentLevel);
	}
}
