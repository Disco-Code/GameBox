#include "Game.h"
#include <SFML\Graphics.hpp>
#include "state/GameState.h"
#include "state/MainMenuState.h"

Game::Game() : m_window(sf::VideoMode(1600, 800), "Gamebox setup test") {
	m_font.loadFromFile("../Resources/Arcon-Regular.otf");
	m_currentState = new MainMenuState(this);
}

Game::~Game() {
	if (m_currentState) {
		delete m_currentState;
	}
	if (m_nextState) {
		delete m_nextState;
	}
}

void Game::handleWindowEvent(const sf::Event& event) {
	m_currentState->handleWindowEvent(event);
}

void Game::update() {
	float dt = m_gameTime.restart().asSeconds();

	ApplyNextState();
	m_currentState->processInput(dt);

	m_window.clear();
	m_currentState->update(dt);
	m_window.display();
}

void Game::SetState(States::ID id) {
	if (m_nextState) {
		delete m_nextState;
		m_nextState = nullptr;
	}
	switch (id) {
		/*case States::None:
			break;*/
	case States::MainMenu:
		m_nextState = new MainMenuState(this);
		break;
	case States::Game:
		m_nextState = new GameState(this);
		break;
	default:
		break;
	}
}

sf::RenderWindow* Game::getWindow() {
	return &m_window;
}

sf::Font& Game::GetFont() {
	return m_font;
}

void Game::ApplyNextState() {
	if (m_nextState) {
		delete m_currentState;
		m_currentState = m_nextState;
		m_nextState = nullptr;
	}
}
