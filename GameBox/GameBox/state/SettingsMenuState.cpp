//#define _CRT_SECURE_NO_WARNINGS - Allow std::strncpy to be used instead of std::strncpy_s 
#define _CRT_SECURE_NO_WARNINGS
#include "SettingsMenuState.h"
#include "../Game.h"

SettingsMenuState::SettingsMenuState(Game* pGame) : State(States::ID::SettingsMenu, pGame) {
	// Get the player's name from the game's settings.
	const std::string& pname = m_game->GetSettings().GetPlayerName();
	// Copy the player's name to the player name buffer, with a maximum length of MAX_PLAYER_NAME_LENGTH - 1 characters.
	std::strncpy(m_playerNameTemp, pname.c_str(), MAX_PLAYER_NAME_LENGTH - 1);
	// Ensure that the player name buffer is null-terminated.
	m_playerNameTemp[MAX_PLAYER_NAME_LENGTH - 1] = '\0';
}

SettingsMenuState::~SettingsMenuState() {
}

void SettingsMenuState::handleWindowEvent(const sf::Event& windowEvent) {
	switch (windowEvent.type) {
	case sf::Event::EventType::KeyPressed:
		if (windowEvent.key.code == sf::Keyboard::Escape) {
			m_game->SetState(States::MainMenu);
		}
		break;
	default:
		break;
	}
}

void SettingsMenuState::processInput(float dt) {
}

void SettingsMenuState::update(float dt) {
}

void SettingsMenuState::renderGUI(float dt) {
	// Get the size of the game window.
	sf::Vector2u windowSize = m_game->getWindow()->getSize();

	// Set the size and position of the next ImGui window to fill the game window.
	ImGui::SetNextWindowSize(ImVec2(windowSize.x, windowSize.y));
	ImGui::SetNextWindowPos(ImVec2(0, 0));

	// Begin a new ImGui window with the specified name, flag, and state.
	if (ImGui::Begin("SettingsMenu", &m_settingMenuOpen, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings)) {
		// Display an input field for the player's name.
		// If the value in the field is changed, update the player's name in the game's settings.
		if (ImGui::InputText("PlayerName", m_playerNameTemp, MAX_PLAYER_NAME_LENGTH - 1)) {
			m_game->GetSettings().SetPlayerName(m_playerNameTemp);
		}
		// End the ImGui window.
		ImGui::End();
	}
	// If the settings menu is closed, switch to the main menu state.
	if (!m_settingMenuOpen) {
		m_game->SetState(States::MainMenu);
	}

}
