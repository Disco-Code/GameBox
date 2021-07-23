#pragma once

#include <sstream>
#include <iostream>
#include "State.h"
#include <functional>

class Button : public sf::Drawable {
public:
	Button(sf::String text, const sf::Font& font, const std::function<void()> func);
	~Button();
	void Trigger();
	void SetIsSelected(bool b);
	void setPosition(float x, float y);

private:
	// Inherited via Drawable
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

	std::function<void()> m_func;
	bool m_isSelected = false;
	sf::Text m_text;
};

class MainMenuState final : public State {
public:
	MainMenuState(Game* pGame);
	~MainMenuState();

	void update(float dt) override;
	void processInput(float dt) override;
	virtual void handleWindowEvent(const sf::Event& windowEvent) override;
private:
	size_t m_selectedButton = 0;
	std::vector<Button> m_buttons;

	sf::Texture m_logoTexture;
	sf::Sprite m_logoSprite;
};