#pragma once

#include <sstream>
#include <iostream>
#include "State.h"

#include "../TextureHandler.h"

class GameState final : public State {
public:
	GameState(Game* pGame);
	~GameState();

	void update(float dt) override;
	void processInput(float dt) override;

	virtual void handleWindowEvent(const sf::Event& windowEvent) override;
	virtual void renderGUI(float dt) override;

private:

	void initializeSystems();
	void initializeAnimations();
	void initializeEntities();

	Game* m_game;

	entityx::Entity m_mapEntity;

	bool m_isLeftMouseButtonDown = false;
	sf::Vector2f m_mouseDownPosition = sf::Vector2f(0, 0);
	sf::FloatRect m_selectionRectangle;
};