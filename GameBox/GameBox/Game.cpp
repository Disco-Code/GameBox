#include "Game.h"
#include <SFML\Graphics.hpp>
#include "state/GameState.h"
#include "state/MainMenuState.h"

#include "thirdparty/imgui/imgui.h"
#include "thirdparty/imgui/imgui-SFML.h"

Game::Game() : m_window(sf::VideoMode(1600, 800), "Gamebox setup test") {
	m_font.loadFromFile("../Resources/Arcon-Regular.otf");
	m_currentState = new MainMenuState(this);
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

	// Update ImGui
	ImGui::SFML::Update(m_window, dt_time);

	ApplyNextState();
	m_currentState->processInput(dt_time.asSeconds());

	m_window.clear();
	m_currentState->update(dt_time.asSeconds());

	//Todo: Remove IMGUI BELOW here. This is just a showcase how to use it.
	ImGui::Begin("SFML-IMGUI Example");
	ImGui::Text("This is a simple example of using SFML with ImGui.");
	if (ImGui::Button("Close"))
		m_window.close();
	ImGui::End();

	ImGui::ShowDemoWindow();
	//Todo: Remove IMGUI ABOVE here. This is just a showcase how to use it.

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
