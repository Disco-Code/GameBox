#include "GameState.h"
#include "../Game.h"

GameState::GameState(Game* pGame) : State(States::Game, pGame) {

	m_BackgroundSprite.setTexture(TextureHandler::getInstance().getTexture("background2.jpg"));
	m_gameObjectList.push_back(new Cannonball(TextureHandler::getInstance().getTexture("player.png")));

	this->stream << 0;
	this->text.setString(stream.str()); //texten i spelet
	this->text.setFont(m_game->GetFont());


	//Setup the Animated Fire
	AnimatedGameObject* m_animatedFireSprite = new AnimatedGameObject(8, 4);
	m_animatedFireSprite->setTexture(TextureHandler::getInstance().getTexture("../Resources/fireSprite.png"));
	m_animatedFireSprite->setPosition(1380, 370);
	m_animatedFireSprite->addAnimation(32);
	m_animatedFireSprite->setScale(2, 2);
	m_gameObjectList.push_back(m_animatedFireSprite);

	//Setup the Animated Character
	AnimatedGameObject* m_animatedCharacterSprite = new AnimatedGameObject(7, 3);
	m_animatedCharacterSprite->setTexture(TextureHandler::getInstance().getTexture("../Resources/character.png"));
	m_animatedCharacterSprite->setPosition(500, 650);
	m_animatedCharacterSprite->addAnimation(7);
	m_animatedCharacterSprite->setAnimationSpeed(16);
	m_animatedCharacterSprite->setAnimation(0);
	m_gameObjectList.push_back(m_animatedCharacterSprite);

	m_tree.Initialize({ 0.0f, 0.0f, (float)m_game->getWindow()->getSize().x, (float)m_game->getWindow()->getSize().y });
	//m_tree.AddGameObject(m_gameObjectList[0]);

	for (auto& i : m_gameObjectList) {
		m_tree.AddGameObject(i);
	}
}

GameState::~GameState() {
	for (auto& i : m_gameObjectList) {
		delete i;
	}
}

void GameState::update(float dt) {

	for (auto& i : m_gameObjectList) {
		i->update(dt);
	}

	m_tree.UpdateTree();

	this->stream.str("test"); // Clear
	this->text.setString(stream.str());	/* Update text with new stream */
}

void GameState::processInput(float dt) {
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
		if (!static_cast<Cannonball*>(m_gameObjectList[0])->getIsAirbourne())
			static_cast<Cannonball*>(m_gameObjectList[0])->shoot();
	}
}


void GameState::handleWindowEvent(const sf::Event& windowEvent) {
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

void GameState::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	// Make sure everything in the game is drawn.
	target.draw(m_BackgroundSprite, states);

	target.draw(m_tree, states);

	for (auto& i : m_gameObjectList) {
		target.draw(*i, states);
	}

	target.draw(this->text);
}