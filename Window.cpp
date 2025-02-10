#include "Window.hpp"

Window::Window(uint32_t width, uint32_t height, const sf::String& title)
	: mWindow(sf::VideoMode(width, height), title, sf::Style::Close) {
	// Enable Vsync and lock FPS to 60
	mWindow.setVerticalSyncEnabled(true);
	mWindow.setFramerateLimit(60);

	sf::View renderWindowView(
		sf::Vector2f(width / 2, height / 2), // Center
		sf::Vector2f((float)width, (float)height)
	); // Size

	mWindow.setView(renderWindowView);

	sf::View renderTexView(
		sf::Vector2f(300, 400), 
		sf::Vector2f((float)width, (float)height)
		//sf::FloatRect(0,0, (float)width, (float)height)
	);

	// Create render texture for this window
	mRenderTex.create(width, height);
	mRenderTex.setView(renderTexView);
}

void Window::Clear() {
	mWindow.clear(sf::Color::Cyan);	
	mRenderTex.clear(sf::Color::Magenta);
}

void Window::Display() {
	mWindow.display();
}

/*
* @brief Polls and handles window events
* @return Whether the Window is still open
*/
bool Window::HandleEvents() {
	sf::Event event;

	while (mWindow.pollEvent(event)) {
		if (event.type == sf::Event::Closed) {
			mWindow.close();
		} else if (event.type == sf::Event::Resized) {
			sf::Vector2u size = sf::Vector2u(event.size.width, event.size.height);

			mWindow.setSize(size);

			//sf::View view(
			//	sf::Vector2f(0.0f, 0.0f), // Center
			//	sf::Vector2f((float)size.x, (float)size.y)); // Size
			sf::View view(sf::FloatRect(0, 0, (float)mWindow.getSize().x, (float)mWindow.getSize().y));
			mWindow.setView(view);

			// Recreate render texture (cannot be resized)
			mRenderTex.create(size.x, size.y);
			mRenderTex.setView(view);
		} else if (event.type == sf::Event::KeyPressed) {
			if (event.key.code == sf::Keyboard::Escape) {
				mWindow.close();
			}
		}
	}

	return mWindow.isOpen();
}
