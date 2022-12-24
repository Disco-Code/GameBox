#pragma once
#include "State.h"
#include "../Settings.h"

class SettingsMenuState : public State {
public:
	SettingsMenuState(Game* pGame);
	~SettingsMenuState();

private:
	// Inherited via State
	virtual void handleWindowEvent(const sf::Event& windowEvent) override;

	virtual void processInput(float dt) override;

	virtual void update(float dt) override;

	virtual void renderGUI(float dt) override;

	char m_playerNameTemp[MAX_PLAYER_NAME_LENGTH] = {};
	bool m_settingMenuOpen = true;
};