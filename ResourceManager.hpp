#pragma once

#include <SFML/Graphics.hpp>

struct Textures {	
	sf::Texture 
		tiles, 
		background, 
		grid, 
		frame, 
		nextTileFrame;
};

struct Fonts {
	sf::Font font;
};

class ResourceManager {
public:
	ResourceManager() {}
	virtual ~ResourceManager() {}

	const Textures& GetTextures() const { return mTextures; }
	const Fonts& GetFonts() const { return mFonts; }

	bool Load();
	bool LoadTextures();
	bool LoadFonts();

private:
	Textures mTextures;	
	Fonts mFonts;
};
