#pragma once

#include <sstream>
#include "../Cannonball.h"
#include <iostream>
#include "State.h"

//#include "../AnimatedGameObject.h"
#include "../TextureHandler.h"
#include "../gameObjects/gameObject.h"
#include "../QuadTree.h"

class GameState final : public State {
public:
	GameState(Game* pGame);
	~GameState();

	void update(float dt) override;
	void processInput(float dt) override;

	virtual void handleWindowEvent(const sf::Event& windowEvent) override;

private:

	std::vector<GameObject*> m_gameObjectList;

	sf::Sprite m_BackgroundSprite;

	sf::Text text;
	std::ostringstream stream;

	QuadTree m_tree;
	entityx::Entity m_playerEntity;
};