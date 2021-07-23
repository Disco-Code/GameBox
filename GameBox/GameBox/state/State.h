#pragma once
#include "SFML/Graphics.hpp"
#include "entityx/entityx.h"

// Forward declare the StateStack
class StateStack;
class Game;

namespace States {

	enum ID : char {
		None,
		MainMenu,
		Game
	};
}

// Base class for all states
class State :public entityx::EntityX {

public:
	State(States::ID id, Game* pGame);
	virtual ~State();

	// Process window events from SFML.
	virtual void handleWindowEvent(const sf::Event& windowEvent) = 0;
	// Process input for the state.
	virtual void processInput(float dt) = 0;
	// Updates the state - runs every frame
	virtual void update(float dt) = 0;

protected:
	Game* m_game;

private:
	States::ID m_id;
};