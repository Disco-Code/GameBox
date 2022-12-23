#pragma once

#include <sstream>
#include <iostream>
#include "State.h"
#include <functional>
#include "Common UI/Button.h"

class MainMenuState final : public State {
public:
	MainMenuState(Game* pGame);
	~MainMenuState();

	void update(float dt) override;
	void processInput(float dt) override;
	virtual void handleWindowEvent(const sf::Event& windowEvent) override;
	virtual void renderGUI(float dt) override;
private:
	/*
		Returns the ID of the button currently being hovered over by the mouse, or -1 if no button is being hovered over
	*/
	size_t getHoveredButtonID();

	size_t m_selectedButton = 0;
	std::vector<Button> m_buttons;

	sf::Texture m_logoTexture;
	sf::Sprite m_logoSprite;
};