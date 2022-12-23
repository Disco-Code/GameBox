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
	// switch statement to handle different types of window events
	switch (windowEvent.type) {

		// case for when a key is pressed
	case sf::Event::EventType::KeyPressed:
		// if the Enter key is pressed, trigger the currently selected button
		if (windowEvent.key.code == sf::Keyboard::Enter) {
			m_buttons[m_selectedButton].Trigger();
		}

		// if the W or Up arrow key is pressed, move the selection up to the previous button if it exists
		if (windowEvent.key.code == sf::Keyboard::W || windowEvent.key.code == sf::Keyboard::Up) {
			if (m_selectedButton > 0) {
				m_buttons[m_selectedButton--].SetIsSelected(false);
				m_buttons[m_selectedButton].SetIsSelected(true);
			}
		}

		// if the S or Down arrow key is pressed, move the selection down to the next button if it exists
		if (windowEvent.key.code == sf::Keyboard::S || windowEvent.key.code == sf::Keyboard::Down) {
			if (m_selectedButton < m_buttons.size() - 1) {
				m_buttons[m_selectedButton++].SetIsSelected(false);
				m_buttons[m_selectedButton].SetIsSelected(true);
			}
		}
		break;

		// case for when a mouse button is pressed
	case sf::Event::MouseButtonPressed:
	{
		// get the ID of the button currently being hovered over
		size_t buttonID = getHoveredButtonID();

		// if a button is being hovered over, trigger it
		if (buttonID != -1) {
			m_buttons[buttonID].Trigger();
		}
		break;
	}
	// case for when the mouse is moved
	case sf::Event::EventType::MouseMoved:
	{
		// get the ID of the button currently being hovered over
		size_t buttonID = getHoveredButtonID();

		// if a button is being hovered over, set it as the selected button
		if (buttonID != -1) {
			m_buttons[m_selectedButton].SetIsSelected(false);
			m_buttons[buttonID].SetIsSelected(true);
			m_selectedButton = buttonID;
		}
		break;
	}
	// default case for handling any other event types not covered in the switch statement
	default:
		break;
	}

}

size_t MainMenuState::getHoveredButtonID() {

	size_t id = 0;
	for (auto& i : m_buttons) {
		sf::Vector2i mousePos = sf::Mouse::getPosition(*m_game->getWindow());
		if (i.GetRectangle().contains(mousePos.x, mousePos.y)) {
			return id;
		}
		id++;
	}

	return -1;
}

void MainMenuState::renderGUI(float dt) {
	ImGui::ShowDemoWindow();
}
