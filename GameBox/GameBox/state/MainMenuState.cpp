#include "MainMenuState.h"
#include "../Game.h"

MainMenuState::MainMenuState(Game* pGame) : State(States::MainMenu, pGame) {

	m_logoTexture.loadFromFile("../Resources/DiscoNDungeons.png");
	m_logoSprite.setTexture(m_logoTexture);

	m_logoSprite.setPosition(-10, 0);

	float posX = 50;
	float posY = 150;

	m_buttons.push_back(Button("Start Game", pGame->GetFont(), [&]() {printf("MainMenu: \"Start Game\" Button Pressed!\n"); m_game->SetState(States::Game); }));
	m_buttons.back().setPosition(posX, posY);
	posY += 50;

	m_buttons.push_back(Button("Settings (not implemented)", pGame->GetFont(), [&]() {printf("MainMenu: \"Settings\" Button Pressed!\n"); }));
	m_buttons.back().setPosition(posX, posY);
	posY += 50;

	m_buttons.push_back(Button("About (not implemented)", pGame->GetFont(), [&]() {printf("MainMenu: \"About\" Button Pressed!\n"); }));
	m_buttons.back().setPosition(posX, posY);
	posY += 50;

	m_buttons.push_back(Button("Quit", pGame->GetFont(), [&]() {printf("MainMenu: \"Exit\" Button Pressed!\n"); m_game->getWindow()->close(); }));
	m_buttons.back().setPosition(posX, posY);
	posY += 50;

	m_buttons[m_selectedButton].SetIsSelected(true);
}

MainMenuState::~MainMenuState() {

}

void MainMenuState::update(float dt) {
	sf::RenderWindow* window = m_game->getWindow();
	window->draw(m_logoSprite);

	for (auto& i : m_buttons) {
		window->draw(i);
	}
}

void MainMenuState::processInput(float dt) {

}

void MainMenuState::handleWindowEvent(const sf::Event& windowEvent) {
	switch (windowEvent.type) {
	case sf::Event::EventType::KeyPressed:
		if (windowEvent.key.code == sf::Keyboard::Enter) {
			m_buttons[m_selectedButton].Trigger();
		}
		if (windowEvent.key.code == sf::Keyboard::W || windowEvent.key.code == sf::Keyboard::Up) {
			if (m_selectedButton > 0) {
				m_buttons[m_selectedButton--].SetIsSelected(false);
				m_buttons[m_selectedButton].SetIsSelected(true);
			}
		}
		if (windowEvent.key.code == sf::Keyboard::S || windowEvent.key.code == sf::Keyboard::Down) {
			if (m_selectedButton < m_buttons.size() - 1) {
				m_buttons[m_selectedButton++].SetIsSelected(false);
				m_buttons[m_selectedButton].SetIsSelected(true);
			}
		}
		break;
	default:
		break;
	}
}

Button::Button(sf::String text, const sf::Font& font, const std::function<void()> func) : m_func(func) {
	m_text.setFont(font);
	m_text.setString(text);
}

Button::~Button() {
}

void Button::Trigger() {
	m_func();
}

void Button::SetIsSelected(bool b) {
	m_isSelected = b;
	if (m_isSelected) {
		m_text.setStyle(sf::Text::Bold | sf::Text::Italic);
	} else {
		m_text.setStyle(0);
	}
}

void Button::setPosition(float x, float y) {
	m_text.setPosition(x, y);
}

void Button::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	if (m_isSelected) {

	} else {

	}
	target.draw(m_text, states);
}
