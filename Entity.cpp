#include "Entity.hpp"

Entity::Entity(World& world, const sf::Texture& texture)
	: mWorld(world)
	, mSprite(texture) { 
}

void Entity::Render(sf::RenderTexture& rt) {
	rt.draw(mSprite);
}
