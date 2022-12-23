#include "LifeTimeSystem.h"
#include "SFML/Graphics.hpp"

LifeTimeSystem::LifeTimeSystem()
{
}

LifeTimeSystem::~LifeTimeSystem()
{
}

void LifeTimeSystem::update(entityx::EntityManager& es, entityx::EventManager& events, entityx::TimeDelta dt)
{

	es.each<LifeTimeComponent, sf::Sprite>([&](entityx::Entity entity, LifeTimeComponent& lifetimecomponent, sf::Sprite& sprite) {

	auto dtime = lifetimecomponent.initialLifetime - lifetimecomponent.lifeTime;
	auto color = sprite.getColor();
	color.a = 255 * (1 - (dtime / lifetimecomponent.initialLifetime));
	sprite.setColor(color);
	lifetimecomponent.lifeTime -= dt;
	if (lifetimecomponent.lifeTime <= 0) {
		entity.destroy();
	}

		});
}
