#include "GameState.h"
#include "../Game.h"
#include "SFML/Graphics.hpp"
#include <unordered_map>

#include "../ECS/Systems.h"

GameState::GameState(Game* pGame) : State(States::Game, pGame) {

	initializeSystems();
	initializeAnimations();
	initializeEntities();

	m_BackgroundSprite.setTexture(TextureHandler::getInstance().getTexture("background2.jpg"));
	m_gameObjectList.push_back(new Cannonball(TextureHandler::getInstance().getTexture("player.png")));

	this->stream << 0;
	this->text.setString(stream.str()); //texten i spelet
	this->text.setFont(m_game->GetFont());
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

	systems.update<AnimationSystem>(dt);
	systems.update<TargetingSystem>(dt);

	sf::RenderWindow* window = m_game->getWindow();
	window->draw(m_BackgroundSprite);
	systems.update<SpriteRenderSystem>(dt);
	for (auto& i : m_gameObjectList) {
		window->draw(*i);
	}

	window->draw(m_tree);
	window->draw(this->text);

	this->stream.str("test"); // Clear
	this->text.setString(stream.str());	/* Update text with new stream */
}

void GameState::processInput(float dt) {
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
		if (!static_cast<Cannonball*>(m_gameObjectList[0])->getIsAirbourne())
			static_cast<Cannonball*>(m_gameObjectList[0])->shoot();
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
		m_playerEntity.component<sf::Sprite>().get()->move(0, -dt * 250);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
		m_playerEntity.component<sf::Sprite>().get()->move(0, dt * 250);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
		m_playerEntity.component<sf::Sprite>().get()->move(-dt * 250, 0);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
		m_playerEntity.component<sf::Sprite>().get()->move(dt * 250, 0);
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

void GameState::initializeSystems() {
	//Setup EntityX Systems
	systems.add<TargetingSystem>();
	auto animationSystem = systems.add<AnimationSystem>();
	systems.add<SpriteRenderSystem>(m_game);
	systems.configure();
}

void GameState::initializeAnimations() {
	//Register all animated sprite sheets here before use

	AnimationSystem* pAnimationSystem = systems.system<AnimationSystem>().get();
	if (pAnimationSystem) {
		//Setup Fire Animation
		{
			AnimatedSheetDescription animDesc(TextureHandler::getInstance().getTexture("../Resources/fireSprite.png").getSize(), 8, 4);
			animDesc.AddAnimation(32);
			pAnimationSystem->AddAnimatedSheetDescription(&TextureHandler::getInstance().getTexture("../Resources/fireSprite.png"), animDesc);
		}
		//Setup Character Animation
		{
			AnimatedSheetDescription animDesc(TextureHandler::getInstance().getTexture("../Resources/character.png").getSize(), 7, 3);
			animDesc.AddAnimation(7);
			pAnimationSystem->AddAnimatedSheetDescription(&TextureHandler::getInstance().getTexture("../Resources/character.png"), animDesc);
		}
	}
}

void GameState::initializeEntities() {
	//===Create Entities===
	{
		//Create Player
		m_playerEntity = entities.create();//Store the player entity in m_playerEntity for future use
		auto comp = m_playerEntity.assign<sf::Sprite>().get();
		comp->setPosition(100, 100);
		comp->setTexture(TextureHandler::getInstance().getTexture("../Resources/default.png"));

		//Create 10 entities that follow the player at different speeds
		for (size_t i = 0; i < 10; i++) {
			entityx::Entity ent2 = entities.create();
			auto spriteComp = ent2.assign<sf::Sprite>().get();
			spriteComp->setPosition(i * 100, i * 100 % 400);
			spriteComp->setTexture(TextureHandler::getInstance().getTexture("default.png"));

			auto targetComp = ent2.assign<TargetingComponent>().get();
			targetComp->target = m_playerEntity;
			targetComp->speed = 35 + i * 5;
		}

		{
			//Create an animated fire entity that follows the player
			entityx::Entity ent2 = entities.create();
			auto spriteComp = ent2.assign<sf::Sprite>().get();
			spriteComp->setPosition(500, 500);
			spriteComp->setTexture(TextureHandler::getInstance().getTexture("../Resources/fireSprite.png"));
			spriteComp->setOrigin(30, 30);

			auto targetComp = ent2.assign<TargetingComponent>().get();
			targetComp->target = m_playerEntity;
			targetComp->speed = 200;

			ent2.assign<AnimationComponent>();
		}
		{
			//Create an animated fire entity that does not follow the player
			entityx::Entity ent2 = entities.create();
			auto spriteComp = ent2.assign<sf::Sprite>().get();
			spriteComp->setPosition(1380, 370);
			spriteComp->setTexture(TextureHandler::getInstance().getTexture("../Resources/fireSprite.png"));
			spriteComp->setScale(2, 2);
			ent2.assign<AnimationComponent>();
		}

		{
			//Create the animated character entity
			entityx::Entity ent2 = entities.create();
			auto spriteComp = ent2.assign<sf::Sprite>().get();
			spriteComp->setPosition(500, 650);
			spriteComp->setTexture(TextureHandler::getInstance().getTexture("../Resources/character.png"));
			auto animationComp = ent2.assign<AnimationComponent>();
			animationComp->m_animationSpeed = 16;
		}
	}

	m_BackgroundSprite.setTexture(TextureHandler::getInstance().getTexture("background2.jpg"));
	m_gameObjectList.push_back(new Cannonball(TextureHandler::getInstance().getTexture("player.png")));

	this->stream << 0;
	this->text.setString(stream.str()); //texten i spelet
	this->text.setFont(m_game->GetFont());

	m_tree.Initialize({ 0.0f, 0.0f, (float)m_game->getWindow()->getSize().x, (float)m_game->getWindow()->getSize().y });


	entities.each<sf::Sprite>([&](entityx::Entity entity, const sf::Sprite& sprite) {
		m_tree.AddGameObject(entity);
		});
}