#pragma once
#include "entityx/entityx.h"
#include "SFML/Graphics/Sprite.hpp"

struct TargetingComponent {
	entityx::Entity target;
	float speed;
};

class TargetingSystem : public entityx::System<TargetingSystem> {
public:
	explicit TargetingSystem();
	~TargetingSystem();
	void update(entityx::EntityManager& es, entityx::EventManager& events, entityx::TimeDelta dt) override;
private:

};