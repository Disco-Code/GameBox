#pragma once
#include "../State.h"
#include "../../Chat.h"

class LobbyState  final : public State {
public:
	LobbyState(Game* pGame);
	~LobbyState();

	// Inherited via State
	virtual void handleWindowEvent(const sf::Event& windowEvent) override;
	virtual void processInput(float dt) override;
	virtual void update(float dt) override;
	virtual void renderGUI(float dt) override;

private:
};

