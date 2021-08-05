#include "FollowTargetSystem.h"

TargetingSystem::TargetingSystem() {
}

TargetingSystem::~TargetingSystem() {
}

void TargetingSystem::update(entityx::EntityManager& es, entityx::EventManager& events, entityx::TimeDelta dt) {
	es.each<TargetingComponent, sf::Sprite>([&](entityx::Entity entity, const TargetingComponent& target, sf::Sprite& sprite) {

		if (target.target.valid()) {
			if (target.target.has_component<sf::Sprite>()) {
				entityx::ComponentHandle<sf::Sprite> spriteCompHND;
				es.unpack<sf::Sprite>(target.target.id(), spriteCompHND);
				spriteCompHND.get()->getPosition();

				sf::Vector2f dir = spriteCompHND.get()->getPosition() - sprite.getPosition();
				float len2 = dir.x * dir.x + dir.y * dir.y;
				if (len2 > 0) {
					dir /= std::sqrt(dir.x * dir.x + dir.y * dir.y);
				}
				sprite.move(dir * target.speed * (float)dt);
			}
		}
		});
}
