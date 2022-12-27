#include "State.h"
#include "../Game.h"

State::State(States::ID id, Game* pGame) {
	m_id = id;
	m_game = pGame;

	//Reset the view for each new state
	m_game->getWindow()->setView(m_game->getWindow()->getDefaultView());
}

State::~State() {
}
