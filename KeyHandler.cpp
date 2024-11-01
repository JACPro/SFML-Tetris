#include "KeyHandler.hpp"

KeyHandler::KeyHandler() : isPressed(false), updateCountdownTimer(0.0f), updateInterval(0.0f), action() {}

KeyHandler::KeyHandler(float updateInterval, std::function<void()> action)
	: isPressed(false), updateCountdownTimer(0.0f), updateInterval(updateInterval), action(action) {}

void KeyHandler::update(sf::Keyboard::Key key, float deltaTime) {
	if (sf::Keyboard::isKeyPressed(key)) {
		if (!isPressed || (updateCountdownTimer -= deltaTime) < 0) { //first press or repeat interval has elapsed
			executeAction();
			updateCountdownTimer = updateInterval;
			isPressed = true;
		}
	} else {
		isPressed = false;
	}
}

void KeyHandler::executeAction() {
	action();
}
