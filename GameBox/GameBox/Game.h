#pragma once

#include "state/State.h"
#include "Networking/Game-Layer/NetworkConnection.h"
#include "entityx/Event.h"
#include "Chat.h"
#include "Settings.h"

class Game {
public:
	Game();
	~Game();

	void handleWindowEvent(const sf::Event& event);
	void update();
	void SetState(States::ID id);

	sf::RenderWindow* getWindow();
	sf::Font& GetFont();

	NetworkConnection m_networkConnection;
	Chat m_chat;

	const Settings& GetSettingsConst() const;
	Settings& GetSettings();

private:
	void ApplyNextState();
	sf::RenderWindow m_window;
	sf::Clock m_gameTime;

	sf::Font m_font;

	State* m_currentState = nullptr;
	State* m_nextState = nullptr;

	entityx::EventManager m_eventManager;
	Settings m_settings;
};