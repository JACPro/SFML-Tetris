#include "KeyHandler.hpp"

KeyHandler::KeyHandler() : mIsPressed(false), mUpdateCountdownTimer(0.0f), mUpdateInterval(0.0f), mOnPressAction() {
}

KeyHandler::KeyHandler(float updateInterval, std::function<void()> onPressAction)
	: mIsPressed(false), mUpdateCountdownTimer(0.0f), mUpdateInterval(updateInterval), mOnPressAction(onPressAction) {
}

KeyHandler::KeyHandler(float updateInterval, std::function<void()> onPressAction, std::function<void()> onReleaseAction)
	: mIsPressed(false), mUpdateCountdownTimer(0.0f), mUpdateInterval(updateInterval), mOnPressAction(onPressAction), 
	mOnReleaseAction(onReleaseAction) { 
}

KeyHandler::KeyHandler(float updateInterval, std::function<void()> onPressAction, std::function<void()> onReleaseAction, 
	std::function<void()> onHeldAction)
	: mIsPressed(false), mUpdateCountdownTimer(0.0f), mUpdateInterval(updateInterval), mOnPressAction(onPressAction), 
	mOnReleaseAction(onReleaseAction), mOnHeldAction(onHeldAction) {
}

void KeyHandler::Update(sf::Keyboard::Key key, float deltaTime) {
	if (sf::Keyboard::isKeyPressed(key)) {
		if (!mIsPressed) { // first press 
			if (mOnPressAction != nullptr) { 
				ExecuteOnPressAction();
			}
			mUpdateCountdownTimer = mUpdateInterval;
			mIsPressed = true;
		} else if ((mUpdateCountdownTimer -= deltaTime) < 0) { // repeat interval has elapsed
			if (mOnHeldAction != nullptr) {
				ExecuteOnHeldAction();
			}
			mUpdateCountdownTimer = mUpdateInterval;
		}
	} else {
		if (mIsPressed) { // just released
			if (mOnReleaseAction != nullptr) {
				ExecuteOnReleaseAction();
			}
			mIsPressed = false;
		}
	}
}

void KeyHandler::ExecuteOnPressAction() {
	mOnPressAction();
}

void KeyHandler::ExecuteOnReleaseAction() {
	mOnReleaseAction();
}

void KeyHandler::ExecuteOnHeldAction() {
	mOnHeldAction();
}

void KeyHandler::AssignNewKeyAction(EKeyboardEvents eventType, std::function<void()> action) {
	switch (eventType) {
	case EKeyboardEvents::Pressed: 
		mOnPressAction = action;
		break;
	case EKeyboardEvents::Released:
		mOnReleaseAction = action;
		break;
	case EKeyboardEvents::Held:
		mOnHeldAction = action;
		break;
	}
}
