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

private:
	bool mIsPressed;
	float mUpdateCountdownTimer;
	float mUpdateInterval;	
	
	std::function<void()> mOnPressAction;
	std::function<void()> mOnHeldAction;
	std::function<void()> mOnReleaseAction;

	void ExecuteOnPressAction();
	void ExecuteOnReleaseAction();
	void ExecuteOnHeldAction();
};