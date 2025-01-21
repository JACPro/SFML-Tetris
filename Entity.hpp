#pragma once

#include "World.hpp"

class Entity {
public:
	Entity(World& world, const sf::Texture& texture);
	virtual ~Entity() {}

	virtual void Render(sf::RenderTexture& rt);

	sf::Sprite& GetSprite() { return mSprite; }

protected:
	World& mWorld;
	sf::Sprite mSprite;
	int renderPriority = 0; // higher = displayed on top
};
