#include <functional>
#include <SFML/Window/Keyboard.hpp>

class KeyHandler {
public:
	KeyHandler();
	KeyHandler(float updateInterval, std::function<void()> action);
	void update(sf::Keyboard::Key key, float deltaTime);

private:
	bool isPressed;
	float updateCountdownTimer;
	float updateInterval;
	std::function<void()> action;

	void executeAction();
};