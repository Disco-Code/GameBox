#include "Game.h"
#include <SFML\Graphics.hpp>
#include "state/GameState.h"
#include "state/MainMenuState.h"
#include "state/Lobby/LobbyState.h"

#include "imgui/imgui.h"
#include "imgui/imgui-SFML.h"

Game::Game() : m_window(sf::VideoMode(1600, 800), "Gamebox setup test") {
	m_font.loadFromFile("../Resources/Arcon-Regular.otf");
	m_currentState = new MainMenuState(this);
	m_networkConnection.Initialize(&m_eventManager);
	m_chat.Initialize(&m_eventManager);
	// Set up ImGui
	ImGui::SFML::Init(m_window);
}

Game::~Game() {
	if (m_currentState) {
		delete m_currentState;
	}
	if (m_nextState) {
		delete m_nextState;
	}

	ImGui::SFML::Shutdown();
}

void Game::handleWindowEvent(const sf::Event& event) {
	ImGui::SFML::ProcessEvent(event);
	m_currentState->handleWindowEvent(event);
}

void Game::update() {
	sf::Time dt_time = m_gameTime.restart();
	float dt_seconds = dt_time.asSeconds();
	// Update ImGui
	ImGui::SFML::Update(m_window, dt_time);

	m_networkConnection.CheckNetworkEvents();

	ApplyNextState();
	m_currentState->processInput(dt_seconds);

	m_window.clear();
	m_currentState->update(dt_seconds); //update and render the state 
	m_currentState->renderGUI(dt_seconds); //render the GUI

	ImGui::SFML::Render(m_window);

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
	case States::Lobby:
		m_nextState = new LobbyState(this);
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
