#pragma once
#include <unordered_map>
#include "gameObjects/gameObject.h"
#include "SFML/Graphics.hpp"
#include "entityx/Entity.h"

class Node : public sf::Drawable, public sf::FloatRect {
public:
	Node();
	~Node();

	Node* m_childNodes = nullptr;
	Node* m_parrentNode = nullptr;

	sf::RectangleShape m_shape_white;
	sf::RectangleShape m_shape_red;

	std::vector<entityx::Entity> m_gameobjects;
	void createChildNodes();

	// Inherited via Drawable
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

	void operator =(const sf::FloatRect& other) {
		left = other.left;
		top = other.top;
		width = other.width;
		height = other.height;

		m_shape_white.setPosition(left, top);
		m_shape_white.setSize(sf::Vector2f(width, height));
		m_shape_white.setFillColor(sf::Color::Transparent);
		m_shape_white.setOutlineThickness(2);

		m_shape_red = m_shape_white;
		m_shape_red.setOutlineColor(sf::Color::Red);
	};
};

class QuadTree : public sf::Drawable {
public:
	QuadTree();
	~QuadTree();

	void Initialize(const sf::FloatRect& rect);
	void AddGameObject(entityx::Entity& entity);
	void UpdateTree();

private:
	void AddGameObjectInternal(entityx::Entity& entity);

	Node* m_rootNode;
	std::unordered_map<entityx::Entity, Node*> m_AllGameObjects;

	// Inherited via Drawable
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

