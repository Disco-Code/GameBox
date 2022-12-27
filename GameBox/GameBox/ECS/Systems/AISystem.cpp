#include "AISystem.h"
//#include "SFML/Graphics/RenderWindow.hpp"
#include "PickingSystem.h"
#include "CollisionSystem.h"

AISystem::AISystem(sf::RenderWindow* window, entityx::EventManager& events, entityx::EntityManager& es, sf::Vector2i mapDims)
	: m_window(window), m_eventmanager(&events), m_entitymanager(&es), Astar(es, window, mapDims.x, mapDims.y)
{
	m_eventmanager = &events;
	m_eventmanager->subscribe<ClickActionEvent>(*this);

}

AISystem::~AISystem()
{
	m_eventmanager->unsubscribe<ClickActionEvent>(*this);
}

void AISystem::update(entityx::EntityManager& es, entityx::EventManager& events, entityx::TimeDelta dt)
{

	// AI stage machine loop
	es.each<AIComponent, sf::Sprite>([&](entityx::Entity entity, AIComponent& aicomponent, sf::Sprite& sprite) {

		if (aicomponent.coolDownTimer <= 0)
		{
			switch (aicomponent.state)
			{
			case AI::State::IDLE:

				break;
			case AI::State::WALKING:

				Walk(es, entity, aicomponent, sprite, dt);
				break;

			case AI::State::ATTACKING:
			{
				if (!aicomponent.targetEntity.valid())
				{
					aicomponent.state = AI::State::IDLE;
					sprite.setTexture(TextureHandler::getInstance().getTexture("../Resources/cubeman.png"));
					if (entity.has_component<SelectedComponent>())
					{
						sprite.setColor(sf::Color::Blue);
					}
					else
					{
						sprite.setColor(sf::Color::White);
					}
					return;
				}

				// Calculate the distance between units
				sf::Sprite* targetSprite = aicomponent.targetEntity.component<sf::Sprite>().get();
				sf::Vector2f spriteCenter = sf::Vector2f(sprite.getPosition().x + sprite.getGlobalBounds().width / 2,
					sprite.getPosition().y + sprite.getGlobalBounds().height / 2);
				sf::Vector2f targetCenter = sf::Vector2f(targetSprite->getPosition().x + targetSprite->getGlobalBounds().width / 2,
					targetSprite->getPosition().y + targetSprite->getGlobalBounds().height / 2);

				sf::Vector2f deltaVec = spriteCenter - targetCenter;
				float realDistance = sqrtf(deltaVec.x * deltaVec.x + deltaVec.y * deltaVec.y)
					- (sprite.getGlobalBounds().width + targetSprite->getGlobalBounds().width) / 2.0f;

				// Compare the distance with the attack range
				if (realDistance < aicomponent.attackRange)
				{
					aicomponent.coolDownTimer = aicomponent.attackspeed;
					
					AIComponent* aicomp = aicomponent.targetEntity.component<AIComponent>().get();
					aicomp->currentHealth -= aicomponent.damage;
					if (aicomp->currentHealth <= 0)
					{
						aicomponent.targetEntity.destroy();
						aicomponent.state = AI::State::IDLE;
						sprite.setTexture(TextureHandler::getInstance().getTexture("../Resources/cubeman.png"));
						if (entity.has_component<SelectedComponent>())
						{
							sprite.setColor(sf::Color::Blue);
						}
						else
						{
							sprite.setColor(sf::Color::White);
						}
					}
				}
				else if (aicomponent.targetPosVector.empty())
				{

					aicomponent.targetPosVector.clear();
					Astar.Solve_AStar(entity.component<sf::Sprite>().get()->getPosition(),
						aicomponent.targetEntity.component<sf::Sprite>().get()->getPosition(), aicomponent.targetPosVector);
				}
				else
				{
					aicomponent.targetPos = aicomponent.targetEntity.component<sf::Sprite>()->getPosition();
					Walk(es, entity, aicomponent, sprite, dt);
				}
			}

			default:
				break;
			}
		}
		else
		{
			aicomponent.coolDownTimer = aicomponent.coolDownTimer - dt;
		}
	});


	// Update all things that are supposed to follow the cursor
	sf::Vector2i selectedNode = Astar.getClosestNodeFromPos(m_window->mapPixelToCoords(sf::Mouse::getPosition(*m_window)));
	sf::Vector2f pos = Astar.getNodeRelativeDistance();
	es.each<FollowMouseComponent, sf::Sprite>([&](entityx::Entity entity, const FollowMouseComponent& followComp, sf::Sprite& sprite) {
		sprite.setPosition(selectedNode.x * pos.x, selectedNode.y * pos.y);
	});


	// Camera movement
	m_entitymanager->each<MapComponent>([&](entityx::Entity entity, MapComponent mapComp) {
		sf::View v = m_window->getView();
		
		if (mapComp.moveLeft)
		{
			if (v.getCenter().x > m_window->getSize().x * 0.4f)
			{
				v.move(-mapComp.moveSpeed * dt, 0);
				m_window->setView(v);
			}
		}
		else if (mapComp.moveRight)
		{
			if (v.getCenter().x < mapComp.width - m_window->getSize().x * 0.4f)
			{
				v.move(mapComp.moveSpeed* dt, 0);
				m_window->setView(v);
			}
		}

		if (mapComp.moveUp)
		{
			if (v.getCenter().y > m_window->getSize().y * 0.4f)
			{
				v.move(0, -mapComp.moveSpeed * dt);
				m_window->setView(v);
			}
		}
		else if (mapComp.moveDown)
		{
			if (v.getCenter().y < mapComp.height - m_window->getSize().y * 0.4f)
			{
				v.move(0, mapComp.moveSpeed* dt);
				m_window->setView(v);
			}
		}

		});
}

void AISystem::receive(const ClickActionEvent& event)
{

	entityx::Entity targetEntity;

	for (entityx::Entity entity : m_entitymanager->entities_with_components<sf::Sprite, SelectableComponent>())
	{
		entityx::ComponentHandle<sf::Sprite> sprite = entity.component<sf::Sprite>();
		if (sprite->getGlobalBounds().contains(event.mousePos))
		{
			targetEntity = entity;
			break;
		}
	}

	if (targetEntity.valid())
	{
		m_entitymanager->each<SelectedComponent, AIComponent, sf::Sprite>([&](entityx::Entity entity, SelectedComponent& selectedComp, AIComponent& AIComp, sf::Sprite& sprite) {
			if (entity != targetEntity)
			{
				AIComp.state = AI::State::ATTACKING;
				sprite.setTexture(TextureHandler::getInstance().getTexture("../Resources/cubemanAngry.png"));
				sprite.setColor(sf::Color::Red);
				AIComp.targetEntity = targetEntity;
				// Empty the vector
				AIComp.targetPosVector.clear();
				Astar.Solve_AStar(entity.component<sf::Sprite>().get()->getPosition(), 
					AIComp.targetEntity.component<sf::Sprite>().get()->getPosition(), AIComp.targetPosVector);
			}
			});
	}
	else
	{
		m_entitymanager->each<AIComponent, SelectedComponent, sf::Sprite>([&](entityx::Entity entity, AIComponent& aicomponent, const SelectedComponent& selectedcomp, sf::Sprite& sprite) {

			auto& comp = *entity.component<AIComponent>().get();
			comp.state = AI::State::WALKING;
			sprite.setTexture(TextureHandler::getInstance().getTexture("../Resources/cubeman.png"));
			sprite.setColor(sf::Color::Blue);

			// Empty the vector
			comp.targetPosVector.clear();
			Astar.Solve_AStar(entity.component<sf::Sprite>().get()->getPosition(), event.mousePos, comp.targetPosVector);

			});
	}

	
}

void AISystem::tempClickTest(const sf::Vector2f& mousePos)
{
	if (Astar.drawNodes)
	{
		// make sure to hit the circleshape or the value will change without the color...
		float dividerX = Astar.mapWidth / Astar.getWidth();
		float dividerY = Astar.mapHeight / Astar.getHeight();

		int nSelectedNodeX = mousePos.x / dividerX;
		int nSelectedNodeY = mousePos.y / dividerY;

		if (nSelectedNodeX >= 0 && nSelectedNodeX < Astar.getWidth())
		{
			if (nSelectedNodeY >= 0 && nSelectedNodeY < Astar.getHeight())
			{
				Astar.nodes[nSelectedNodeY * Astar.getWidth() + nSelectedNodeX].bObstacle = !Astar.nodes[nSelectedNodeY * Astar.getWidth() + nSelectedNodeX].bObstacle;
				std::cout << Astar.nodes[nSelectedNodeY * Astar.getWidth() + nSelectedNodeX].vecNeighbours.size() << "   " << std::endl;

				m_entitymanager->each<sf::CircleShape>([&](entityx::Entity testedEntity, sf::CircleShape& shape) {

					if (shape.getGlobalBounds().contains(mousePos)) {
						if (Astar.nodes[nSelectedNodeY * Astar.getWidth() + nSelectedNodeX].bObstacle)
						{
							shape.setFillColor(sf::Color::Red);
						}
						else
						{
							shape.setFillColor(sf::Color::Green);
						}
					}
					});
			}
		}
	}
}

void AISystem::DrawNodes()
{
	if (Astar.drawNodes)
	{
		Astar.DrawNodes();
	}
}

void AISystem::Walk(entityx::EntityManager& es, entityx::Entity entity, AIComponent& aicomponent, sf::Sprite& sprite, entityx::TimeDelta dt)
{
	sf::Vector2f dir;
	float length;
	sf::Vector2f targPos;
	//sprite.setColor(sf::Color::Blue);
	if (aicomponent.targetPosVector.empty())
	{
		aicomponent.state = AI::State::IDLE;
		sprite.setTexture(TextureHandler::getInstance().getTexture("../Resources/cubeman.png"));
		if (!entity.has_component<SelectedComponent>())
		{
			sprite.setColor(sf::Color::White);
		}
		return;
	}

	// Get direction
	targPos = aicomponent.targetPosVector.back();
	dir = targPos - sprite.getPosition();
	// Normalize
	length = sqrtf(dir.x * dir.x + dir.y * dir.y);
	if (length > 5.0f)
	{
		dir.x = dir.x / length;
		dir.y = dir.y / length;
		//std::cout << dir.y << std::endl;
		// Move
		float totalSpeed = (float)(aicomponent.speed * dt);
		//std::cout << totalSpeed << std::endl;

		sprite.move(dir.x * totalSpeed, dir.y * totalSpeed);


		/*es.each<CollisionComponent, sf::Sprite>([&](entityx::Entity testedEntity, const CollisionComponent& collisionComponent, const sf::Sprite& testedSprite) {

			if (sprite.getGlobalBounds().intersects(testedSprite.getGlobalBounds()) && entity != testedEntity) {

				sprite.move(-dir.x * totalSpeed, -dir.y * totalSpeed);

			}
			});*/
	}
	else
	{
		aicomponent.targetPosVector.pop_back();
	}
}
