#pragma once

#include "Window.hpp"
#include "ResourceManager.hpp"
#include "ScreenFactory.hpp"

class Entity;


class World {
public:
	explicit World(Window& window);

	bool Load();
	void Update(float deltaTime);
	void Render();
	void Shutdown();

	const ResourceManager& GetResources() const { return mResourceManager; }
	sf::RenderTexture& GetRenderTex() const { return mWindow.GetRenderTex(); }

private:
	Window& mWindow;
	ResourceManager mResourceManager;	

	ScreenFactory mScreenFactory;
	std::unique_ptr<IScreen> mCurrentScreen;
};
