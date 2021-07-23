#include "GameState.h"
#include "../Game.h"
#include "SFML/Graphics.hpp"
#include <unordered_map>

struct TargetingComponent {
	entityx::Entity target;
	float speed;
};

struct AnimationComponent {
	float m_animationSpeed = 24;
	float m_currentAnimationFrame;
	size_t m_currentAnimationIndex;

	void SetAnimationTime(float time) {
		m_currentAnimationFrame = time * m_animationSpeed;
	};
	float GetAnimationTime() {
		return m_currentAnimationFrame / m_animationSpeed;
	};
};

struct AnimationDescription {
	size_t nFrames;
	size_t animationRowStart;
};

struct AnimatedSheetDescription {
	AnimatedSheetDescription() {
	};
	AnimatedSheetDescription(const sf::Vector2u& textureSize, size_t nColumns, size_t nRows) {
		m_nTextureColumns = nColumns;
		m_nTextureRows = nRows;

		m_animationFrameWidth = textureSize.x / m_nTextureColumns;
		m_animationFrameHeight = textureSize.y / m_nTextureRows;
	};

	size_t AddAnimation(size_t nFrames) {
		size_t index = m_animations.size();

		AnimationDescription desc = {};
		desc.nFrames = nFrames;
		desc.animationRowStart = m_nextAnimationRow;

		m_nextAnimationRow += nFrames / m_nTextureColumns + (nFrames % m_nTextureColumns != 0);

		m_animations.push_back(desc);

		return index;
	};

	const AnimationDescription& GetAnimationDescription(size_t index) const {
		if (index < m_animations.size()) {
			return m_animations.at(index);
		}
		return m_animations.back();
	};

	size_t GetFrameWidth() const {
		return m_animationFrameWidth;
	};
	size_t GetFrameHeight() const {
		return m_animationFrameHeight;
	};
	size_t GetnColumns() const {
		return m_nTextureColumns;
	};
	size_t GetnRows() const {
		return m_nTextureRows;
	};


private:
	std::vector<AnimationDescription> m_animations;
	size_t m_nextAnimationRow = 0;
	size_t m_animationFrameWidth = 0;
	size_t m_animationFrameHeight = 0;
	size_t m_nTextureColumns = 0;
	size_t m_nTextureRows = 0;
};

class AnimationSystem : public entityx::System<AnimationSystem> {
public:
	AnimationSystem() {
	};
	~AnimationSystem() {
	};

	void update(entityx::EntityManager& es, entityx::EventManager& events, entityx::TimeDelta dt) override {
		es.each<AnimationComponent, sf::Sprite>([&](entityx::Entity entity, AnimationComponent& animationData, sf::Sprite& sprite) {

			const AnimatedSheetDescription* animationSheetDesc = GetAnimatedSheetDescription(sprite.getTexture());

			if (animationSheetDesc) {
				const AnimationDescription& animationDescription = animationSheetDesc->GetAnimationDescription(animationData.m_currentAnimationIndex);

				animationData.m_currentAnimationFrame += dt * animationData.m_animationSpeed;
				size_t nextFrame = ((size_t)animationData.m_currentAnimationFrame) % animationDescription.nFrames;

				//Update Sprite Texture Rect
				sf::IntRect rect;

				size_t column = nextFrame % animationSheetDesc->GetnColumns();
				size_t row = animationDescription.animationRowStart + nextFrame / animationSheetDesc->GetnColumns();

				rect.left = column * animationSheetDesc->GetFrameWidth();
				rect.width = animationSheetDesc->GetFrameWidth();
				rect.top = row * animationSheetDesc->GetFrameHeight();
				rect.height = animationSheetDesc->GetFrameHeight();

				sprite.setTextureRect(rect);
			}
			});
	}

	void AddAnimatedSheetDescription(const sf::Texture* texture, const AnimatedSheetDescription& desc) {
		m_animatedSheets[texture] = desc;
	};

private:

	const AnimatedSheetDescription* GetAnimatedSheetDescription(const sf::Texture* texture) {
		if (m_animatedSheets.count(texture)) {
			return &m_animatedSheets[texture];
		} else {
			return nullptr;
		}
	};

	std::unordered_map<const sf::Texture*, AnimatedSheetDescription> m_animatedSheets;
};

class TargetingSystem : public entityx::System<TargetingSystem> {
public:
	explicit TargetingSystem(Game* pGame) : m_pGame(pGame) {
	};

	~TargetingSystem() {
	};

	void update(entityx::EntityManager& es, entityx::EventManager& events, entityx::TimeDelta dt) override {
		es.each<TargetingComponent, sf::Sprite>([&](entityx::Entity entity, const TargetingComponent& target, sf::Sprite& sprite) {

			if (target.target.valid()) {
				if (target.target.has_component<sf::Sprite>()) {
					entityx::ComponentHandle<sf::Sprite> spriteCompHND;
					es.unpack<sf::Sprite>(target.target.id(), spriteCompHND);
					spriteCompHND.get()->getPosition();

					sf::Vector2f dir = spriteCompHND.get()->getPosition() - sprite.getPosition();
					float len2 = dir.x * dir.x + dir.y * dir.y;
					if (len2 > 0) {
						dir /= std::sqrt(dir.x * dir.x + dir.y * dir.y);
					}
					sprite.move(dir * target.speed * (float)dt);
				}
			}
			});
	}
private:
	Game* m_pGame;
};

class SpriteRenderSystem : public entityx::System<SpriteRenderSystem> {
public:
	explicit SpriteRenderSystem(Game* pGame) : m_pGame(pGame) {
	};

	~SpriteRenderSystem() {
	};

	void update(entityx::EntityManager& es, entityx::EventManager& events, entityx::TimeDelta dt) override {
		es.each<sf::Sprite>([&](entityx::Entity entity, const sf::Sprite& sprite) {
			m_pGame->getWindow()->draw(sprite);
			});
	}
private:
	Game* m_pGame;
};

GameState::GameState(Game* pGame) : State(States::Game, pGame) {

	//======================================
	//Setup EntityX Systems
	systems.add<TargetingSystem>(pGame);
	auto animationSystem = systems.add<AnimationSystem>();
	systems.add<SpriteRenderSystem>(pGame);
	systems.configure();
	//======================================

	//Setup Fire Animation
	{
		AnimatedSheetDescription asd(TextureHandler::getInstance().getTexture("../Resources/fireSprite.png").getSize(), 8, 4);
		asd.AddAnimation(32);
		animationSystem->AddAnimatedSheetDescription(&TextureHandler::getInstance().getTexture("../Resources/fireSprite.png"), asd);
	}
	//Setup Character Animation
	{
		AnimatedSheetDescription asd(TextureHandler::getInstance().getTexture("../Resources/character.png").getSize(), 7, 3);
		asd.AddAnimation(7);
		animationSystem->AddAnimatedSheetDescription(&TextureHandler::getInstance().getTexture("../Resources/character.png"), asd);
	}

	//===Create Entities===
	{
		m_playerEntity = entities.create();
		auto comp = m_playerEntity.assign<sf::Sprite>().get();
		comp->setPosition(100, 100);
		comp->setTexture(TextureHandler::getInstance().getTexture("../Resources/default.png"));

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
			//Create Fire Entity
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
			//Create Another Fire Entity
			entityx::Entity ent2 = entities.create();
			auto spriteComp = ent2.assign<sf::Sprite>().get();
			spriteComp->setPosition(1380, 370);
			spriteComp->setTexture(TextureHandler::getInstance().getTexture("../Resources/fireSprite.png"));
			spriteComp->setScale(2, 2);
			ent2.assign<AnimationComponent>();
		}

		{
			//Create Character Entity
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