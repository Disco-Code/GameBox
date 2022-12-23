#pragma once
#include "entityx/entityx.h"
#include "SFML/Graphics/CircleShape.hpp"
#include "../../events/ClickActionEvent.h"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Sprite.hpp"
#include "../../TextureHandler.h"
#include <list>

//namespace sf {
//	class RenderWindow;
//}

namespace AI {
	enum State
	{
		IDLE,
		WALKING,
		ATTACKING
	};
}

struct AIComponent
{
	// Ball entity
	AI::State state = AI::State::IDLE;
	sf::Vector2f targetPos;
	entityx::Entity targetEntity;
	float speed = 100.0f;
	float attackspeed = 1.0f;
	float coolDownTimer = 0.0f; // used in state machine
	std::vector<sf::Vector2f> targetPosVector;
	uint32_t attackRange = 10.0f;

	//Stats
	uint32_t maxHealth = 3;
	uint32_t currentHealth = maxHealth;
	uint32_t damage = 1;
};

// Put the map here until i figure out a better place
struct MapComponent
{
	
	MapComponent(int w = 0, int h = 0) {
		width = w;
		height = h;
	}
	unsigned int width;
	unsigned int height;
	bool moveLeft = false;
	bool moveRight = false;
	bool moveUp = false;
	bool moveDown = false;
	float moveSpeed = 700.0f;
};

struct FollowMouseComponent
{
};

class AStarPathfinding
{
public:
	
	AStarPathfinding(entityx::EntityManager& es, sf::RenderWindow* window, unsigned int mapWidth, unsigned int mapHeight) 
		: m_entitymanager(&es), m_window(window){

		std::cout << "Loading map size, width: " << mapWidth << " , height: "
			<< mapHeight << std::endl;

		Create(MapComponent(mapWidth, mapHeight));
	};
	~AStarPathfinding() {
		delete[] nodes;

		if (drawNodes) {
			delete[] visualNodes;
		}
	};
	// how many horizontal nodes 
	int const getWidth() { return nManWidth; };
	// -||- vertical
	int const getHeight() { return nManHeight; };
	
	bool drawNodes = true;
	int mapWidth;
	int mapHeight;

private:
	struct sNode
	{
		bool bObstacle = false;		// Is the node an obstruction?
		bool bVisited = false;		// Have we searched for this noce before?
		float fGlobalGoal;			// Distance to goal so far
		float fLocalGoal;			// Distance to goal if we took the alternative
		int x;						// Nodes position in 2D space
		int y;
		std::vector<sNode*> vecNeighbours;	// Connections to teighbours
		sNode* parent;				// Node connecting to this node that offers sh
	};

	
	// number of nodes in each dimension
	int nManWidth = 64;
	int nManHeight = 64; // strange things happens when 8



	sNode* nodeStart = nullptr;
	sNode* nodeEnd = nullptr;

	// to draw the nodes
	entityx::Entity* visualNodes = nullptr;
	entityx::EntityManager* m_entitymanager;
	sf::RenderWindow* m_window;

public:
	sNode* nodes = nullptr;

	// Send in map dimensions
	void Create(MapComponent window) {

		mapWidth = window.width;
		mapHeight = window.height;

		nodes = new sNode[((size_t)nManWidth * nManHeight)];
		float posScaleX = mapWidth / nManWidth;
		float posScaleY = mapHeight / nManHeight;
		for (size_t x = 0; x < nManWidth; x++)
		{
			for (size_t y = 0; y < nManHeight; y++)
			{
				nodes[y * nManWidth + x].x = x * posScaleX;
				nodes[y * nManWidth + x].y = y * posScaleY;
				nodes[y * nManWidth + x].bObstacle = false;
				nodes[y * nManWidth + x].parent = nullptr;
				nodes[y * nManWidth + x].bVisited = false;
			}
		}
		/*if (drawNodes)
		{
			visualNodes = new entityx::Entity[nManWidth * (size_t)nManHeight];
			for (int x = 0; x < nManWidth; x++)
			{
				for (int y = 0; y < nManHeight; y++)
				{
					visualNodes[y * nManWidth + x] = m_entitymanager->create();
					auto shape = visualNodes[y * nManWidth + x].assign<sf::CircleShape>(5.0f).get();
					shape->setPosition(x * posScaleX, y * posScaleY);
					shape->setFillColor(sf::Color::Green);
				}
			}
		}*/

		// Create connections
		for (int x = 0; x < nManWidth; x++)
		{
			for (int y = 0; y < nManHeight; y++)
			{
				size_t id = (size_t)y * nManWidth + x;
				std::vector<sNode*>& tmpVec = nodes[id].vecNeighbours;
				if (y > 0)
					tmpVec.push_back(&nodes[(y - 1) * nManWidth + (x + 0)]);
				if (y < nManHeight - 1)
					tmpVec.push_back(&nodes[(y + 1) * nManWidth + (x + 0)]);
				if (x > 0)
					tmpVec.push_back(&nodes[(y + 0) * nManWidth + (x - 1)]);
				if (x < nManWidth - 1)
					tmpVec.push_back(&nodes[(y + 0) * nManWidth + (x + 1)]);

				// We can also connect diagonally
				if (y > 0 && x > 0)
					nodes[y* nManWidth + x].vecNeighbours.push_back(&nodes[(y - 1) * nManWidth + (x - 1)]);
				if (y < nManHeight - 1 && x > 0)
					nodes[y* nManWidth + x].vecNeighbours.push_back(&nodes[(y + 1) * nManWidth + (x - 1)]);
				if (y > 0 && x < nManWidth - 1)
					nodes[y* nManWidth + x].vecNeighbours.push_back(&nodes[(y - 1) * nManWidth + (x + 1)]);
				if (y < nManHeight - 1 && x < nManWidth - 1)
					nodes[y* nManWidth + x].vecNeighbours.push_back(&nodes[(y + 1) * nManWidth + (x + 1)]);
				

			}
		}

		nodeStart = &nodes[(nManHeight / 2) * nManWidth + 1];
		nodeEnd = &nodes[(nManHeight / 2) * nManWidth + nManWidth - 2];

		std::cout << "Astar created " << nManWidth * nManHeight << " nodes, (" << nManWidth
			<< ") (" << nManHeight << ")" << std::endl;

	};

	void DrawNodes() {
		float posScaleX = mapWidth / nManWidth;
		float posScaleY = mapHeight / nManHeight;

		visualNodes = new entityx::Entity[nManWidth * (size_t)nManHeight];
		for (int x = 0; x < nManWidth; x++)
		{
			for (int y = 0; y < nManHeight; y++)
			{
				visualNodes[y * nManWidth + x] = m_entitymanager->create();
				auto shape = visualNodes[y * nManWidth + x].assign<sf::CircleShape>(5.0f).get();
				shape->setPosition(x * posScaleX, y * posScaleY);
				shape->setFillColor(sf::Color::Green);
			}
		}
	};

	// Used to get the distance between nodes.
	sf::Vector2f getNodeRelativeDistance() { 
		return sf::Vector2f(mapWidth / nManWidth, mapHeight / nManHeight);
	};

	sf::Vector2i getClosestNodeFromPos(sf::Vector2f INpos) {
		float dividerX = mapWidth / nManWidth;
		float dividerY = mapHeight / nManHeight;
		float offset = 0.49f; // As it's an int, this makes it round up sometimes

		sf::Vector2f pos = INpos;

		if (pos.x > mapWidth)
		{
			pos.x = mapWidth - 1;
		}
		else if (pos.x < 0)
		{
			pos.x = 0;
		}

		if (pos.y > mapHeight)
		{
			pos.y = mapHeight - 1;
		}
		else if (pos.y < 0)
		{
			pos.y = 0;
		}

		pos.x = std::min(nManWidth - 1.0f, pos.x / dividerX + offset);
		pos.y = std::min(nManHeight - 1.0f, pos.y / dividerY + offset);

		return sf::Vector2i((int)pos.x, (int)pos.y);
	};

	void Solve_AStar(sf::Vector2f startPos, sf::Vector2f targetPos, std::vector<sf::Vector2f>& posVector) {

		// Determine start and end node from the start and target postition
		sf::Vector2i selectedStartNode = getClosestNodeFromPos(startPos);
		nodeStart = &nodes[selectedStartNode.y * nManHeight + selectedStartNode.x];

		sf::Vector2i selectedEndNode = getClosestNodeFromPos(targetPos);
		nodeEnd = &nodes[selectedEndNode.y * nManHeight + selectedEndNode.x];


		// Reset navigation graph - default all node states
		for (int x = 0; x < nManWidth; x++)
		{
			for (int y = 0; y < nManHeight; y++)
			{
				nodes[y * nManWidth + x].fGlobalGoal = INFINITY;
				nodes[y * nManWidth + x].fLocalGoal = INFINITY;
				nodes[y * nManWidth + x].parent = nullptr;
				nodes[y * nManWidth + x].bVisited = false;
			}
		}

		auto distance = [](sNode* a, sNode* b) // For convenience
		{
			return sqrtf((a->x - b->x) * (a->x - b->x) + (a->y - b->y) * (a->y - b->y));
		};

		auto heuristic = [distance](sNode* a, sNode* b)
		{
			return distance(a, b);
		};

		// Setup start conditions
		sNode* nodeCurrent = nodeStart;
		nodeStart->fLocalGoal = 0.0f;
		nodeStart->fGlobalGoal = heuristic(nodeStart, nodeEnd);


		// Add start node to not tested list - this will ensure it gets tested.
		// As the algorithm progresses, newly discovered nodes get added to this
		// list, and will themselves be tested later
		std::list<sNode*> listNotTestedNodes;
		listNotTestedNodes.push_back(nodeStart);

		// if the not tested list contains nodes, there may be better paths
		// which have not yet been explored. However, we will also stop 
		// searching when we reach the target - there may well be better
		// paths but this one will do - it wont be the longest.
		while (!listNotTestedNodes.empty() && nodeCurrent != nodeEnd)// Find absolutely shortest path // && nodeCurrent != nodeEnd)
		{
			// Sort Untested nodes by global goal, so lowest is first
			listNotTestedNodes.sort([](const sNode* lhs, const sNode* rhs) { return lhs->fGlobalGoal < rhs->fGlobalGoal; });

			// Front of listNotTestedNodes is potentially the lowest distance node. Our
			// list may also contain nodes that have been visited, so ditch these...
			while (!listNotTestedNodes.empty() && listNotTestedNodes.front()->bVisited)
				listNotTestedNodes.pop_front();

			// ...or abort because there are no valid nodes left to test
			if (listNotTestedNodes.empty())
				break;

			nodeCurrent = listNotTestedNodes.front();
			nodeCurrent->bVisited = true; // We only explore a node once


			// Check each of this node's neighbours...
			for (auto nodeNeighbour : nodeCurrent->vecNeighbours)
			{
				// ... and only if the neighbour is not visited and is 
				// not an obstacle, add it to NotTested List
				if (!nodeNeighbour->bVisited && nodeNeighbour->bObstacle == 0)
					listNotTestedNodes.push_back(nodeNeighbour);

				// Calculate the neighbours potential lowest parent distance
				float fPossiblyLowerGoal = nodeCurrent->fLocalGoal + distance(nodeCurrent, nodeNeighbour);

				// If choosing to path through this node is a lower distance than what 
				// the neighbour currently has set, update the neighbour to use this node
				// as the path source, and set its distance scores as necessary
				if (fPossiblyLowerGoal < nodeNeighbour->fLocalGoal)
				{
					nodeNeighbour->parent = nodeCurrent;
					nodeNeighbour->fLocalGoal = fPossiblyLowerGoal;

					// The best path length to the neighbour being tested has changed, so
					// update the neighbour's score. The heuristic is used to globally bias
					// the path algorithm, so it knows if its getting better or worse. At some
					// point the algo will realise this path is worse and abandon it, and then go
					// and search along the next best path.
					nodeNeighbour->fGlobalGoal = nodeNeighbour->fLocalGoal + heuristic(nodeNeighbour, nodeEnd);
				}
			}
		}

		posVector.clear();
		// Om noder i rad, ta bort noder i mitten av raden
		if (nodeEnd != nullptr)
		{
			sNode* p = nodeEnd;
			while (p->parent != nullptr)
			{
				posVector.push_back(sf::Vector2f(p->x, p->y));
				p = p->parent;
			}
		}
	}
};


class AISystem : public entityx::System<AISystem>, public entityx::Receiver<AISystem>
{
public:
	explicit AISystem(sf::RenderWindow* window, entityx::EventManager& events, entityx::EntityManager& es, sf::Vector2i mapDims);
	~AISystem();
	void update(entityx::EntityManager& es, entityx::EventManager& events, entityx::TimeDelta dt) override;

	void receive(const ClickActionEvent& event);

	// used to debug A*
	void tempClickTest(const sf::Vector2f& mousePos);
	// used to debug A*, visualize each node
	void DrawNodes();


private:

	void Walk(entityx::EntityManager& es, entityx::Entity entity, AIComponent& aicomponent, sf::Sprite& sprite, entityx::TimeDelta dt);
	
	sf::RenderWindow* m_window;
	entityx::EntityManager* m_entitymanager;
	entityx::EventManager* m_eventmanager;

	AStarPathfinding Astar;

};