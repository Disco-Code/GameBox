#include "SpriteRenderSystem.h"

SpriteRenderSystem::SpriteRenderSystem(Game* pGame) : m_pGame(pGame) {
}

SpriteRenderSystem::~SpriteRenderSystem() {
}

void SpriteRenderSystem::update(entityx::EntityManager& es, entityx::EventManager& events, entityx::TimeDelta dt) {
	es.each<sf::Sprite>([&](entityx::Entity entity, const sf::Sprite& sprite) {
		m_pGame->getWindow()->draw(sprite);
		});
}
