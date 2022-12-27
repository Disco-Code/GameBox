#pragma once
#include <entityx/entityx.h>

struct LifeTimeComponent {
	float initialLifetime;
	float lifeTime;
};

class LifeTimeSystem : public entityx::System<LifeTimeSystem> {
public:
	explicit LifeTimeSystem();
	~LifeTimeSystem();
	void update(entityx::EntityManager& es, entityx::EventManager& events, entityx::TimeDelta dt) override;
private:

};