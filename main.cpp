#include <SFML/Graphics.hpp>

#include <time.h>

#include "Window.hpp"
#include "World.hpp"

int main() {
	srand(time(0));

	Window window(ScreenLayout::SCREEN_WIDTH, ScreenLayout::SCREEN_HEIGHT, "SFML TETRIS");
	World world(window);

	if (world.Load()) {
		sf::Clock clock;

		while (window.HandleEvents()) {
			sf::Time deltaTime = clock.getElapsedTime();
			clock.restart();

			window.Clear();

			world.Update(deltaTime.asSeconds());
			world.Render();

			window.Display();
		}
	} else {
		printf("ERROR: Failed to load resources\r\n");
	}

	world.Shutdown();	

	return 0;
}
