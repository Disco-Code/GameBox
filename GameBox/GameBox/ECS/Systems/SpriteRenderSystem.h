#pragma once
#include "entityx/entityx.h"
#include "SFML/Graphics.hpp"
#include "../../Game.h"

class SpriteRenderSystem : public entityx::System<SpriteRenderSystem> {
public:
	explicit SpriteRenderSystem(Game* pGame);
	~SpriteRenderSystem();
	void update(entityx::EntityManager& es, entityx::EventManager& events, entityx::TimeDelta dt) override;
private:
	Game* m_pGame;
};