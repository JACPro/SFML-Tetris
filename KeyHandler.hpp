#pragma once

#include <functional>
#include <SFML/Window/Keyboard.hpp>
#include "EKeyboardEvents.hpp"

class KeyHandler {
public:
	bool mHandleWhilePaused = false;

	KeyHandler();
	KeyHandler(float updateInterval, std::function<void()> onPressAction);
	KeyHandler(float updateInterval, std::function<void()> onPressAction, std::function<void()> onReleaseAction);
	KeyHandler(float updateInterval, std::function<void()> onPressAction, std::function<void()> onReleaseAction, 
		std::function<void()> onHeldAction);

	void Update(sf::Keyboard::Key key, float deltaTime);

	void AssignNewKeyAction(EKeyboardEvents eventType, std::function<void()> action);
	
	/*
	* @brief Set how long to wait between initial press event and first instance of repeat event
	* @param delay - how long of a delay (in seconds) to wait after initial press before calling repeat event for first time
	*/
	void SetFirstHeldRepeatDelay(float delay);

private:
	bool mIsPressed;

	// For firing repeat inputs when held
	float mUpdateCountdownTimer;
	float mUpdateInterval;	

	// For different duration before first repeat when held
	float mFirstUpdateInterval;
	
	std::function<void()> mOnPressAction;
	std::function<void()> mOnHeldAction;
	std::function<void()> mOnReleaseAction;

	void ExecuteOnPressAction();
	void ExecuteOnReleaseAction();
	void ExecuteOnHeldAction();
};