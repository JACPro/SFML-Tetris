#pragma once

#include <SFML/Graphics.hpp>

class Window {
public:
	Window(uint32_t width, uint32_t height, const sf::String& title);

	void Clear();
	void Display();
	bool HandleEvents();

	void Close() { mWindow.close(); }

	sf::RenderWindow& GetRenderWindow() { return mWindow; }
	sf::RenderTexture& GetRenderTex() { return mRenderTex; }

private:
	sf::RenderWindow mWindow;
	sf::RenderTexture mRenderTex;

};