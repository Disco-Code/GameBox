#include "QuadTree.h"

QuadTree::QuadTree() {
}

QuadTree::~QuadTree() {
	if (m_rootNode) {
		delete m_rootNode;
	}
}

void QuadTree::Initialize(const sf::FloatRect& rect) {
	m_rootNode = new Node;
	*m_rootNode = rect;
}

void QuadTree::AddGameObject(GameObject* gameObject) {
	if (!gameObject) {
		return;
	}

	AddGameObjectInternal(gameObject);
}

void QuadTree::UpdateTree() {
	delete[] m_rootNode->m_childNodes;
	m_rootNode->m_childNodes = nullptr;
	m_rootNode->m_gameobjects.clear();
	for (auto& i : m_AllGameObjects) {
		AddGameObjectInternal(i.first);
	}
}

void QuadTree::AddGameObjectInternal(GameObject* gameObject) {
	Node* currentNode = m_rootNode;
	Node* intersectedNode = nullptr;
	const sf::FloatRect& rect = gameObject->getGlobalBounds();
	bool finalNode = false;

	while (finalNode == false) {
		if (!currentNode->m_childNodes) {
			currentNode->createChildNodes();
		}

		for (size_t i = 0; i < 4; i++) {
			if (currentNode->m_childNodes[i].intersects(rect)) {
				if (intersectedNode) {
					//Game object is intersection more than one child node => Add the game object to the parent node.
					intersectedNode = currentNode;
					finalNode = true;
					break;
				} else {
					//Store the first child node that the game object intersects. If no other child nodes intersects the game object, this node is selected to store it.
					intersectedNode = &currentNode->m_childNodes[i];
					finalNode = false;
				}
			}
		}

		if (intersectedNode) {
			currentNode = intersectedNode;
			intersectedNode = nullptr;
		} else {
			//Game Object is outside the current Quadtree
			//TODO: Expand the Quadtree accordingly
			//Tempfix
			intersectedNode = currentNode;
			finalNode = true;
		}
	}

	if (currentNode) {
		//Store a pointer to the game object in the correct node;
		currentNode->m_gameobjects.push_back(gameObject);
		m_AllGameObjects[gameObject] = currentNode;
	}
}

void QuadTree::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	if (m_rootNode) {
		target.draw(*m_rootNode, states);
	}
}

Node::Node() {
}

Node::~Node() {
	if (m_childNodes) {
		delete[] m_childNodes;
	}
}

void Node::createChildNodes() {
	if (m_childNodes) {
		return;
	}
	m_childNodes = new Node[4];

	float halfWidth = width / 2;
	float halfHeight = height / 2;
	m_childNodes[0] = { left, top, halfWidth, halfHeight };
	m_childNodes[1] = { left + halfWidth, top, halfWidth, halfHeight };
	m_childNodes[2] = { left, top + halfHeight, halfWidth, halfHeight };
	m_childNodes[3] = { left + halfWidth, top + halfHeight, halfWidth, halfHeight };

	for (size_t i = 0; i < 4; i++) {
		m_childNodes->m_parrentNode = this;
	}
}

void Node::draw(sf::RenderTarget& target, sf::RenderStates states) const {

	if (m_gameobjects.empty()) {
		//target.draw(m_shape_red, states);
	} else {
		target.draw(m_shape_white, states);
	}
	if (m_childNodes) {
		for (size_t i = 0; i < 4; i++) {
			target.draw(m_childNodes[i], states);
		}
	}
}
