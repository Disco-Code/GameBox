#include "AnimatedSprite.h"

AnimatedSprite::AnimatedSprite(size_t nColumns, size_t nRows) {
	m_nTextureColumns = nColumns;
	m_nTextureRows = nRows;
}

AnimatedSprite::~AnimatedSprite() {
}

void AnimatedSprite::update(float dt) {
	m_currentAnimationTime += dt * m_animationSpeed;
	size_t nextFrame = ((size_t)m_currentAnimationTime) % m_currentAnimation.nFrames;

	if (nextFrame != m_currentAnimationFrame) {
		m_currentAnimationFrame = nextFrame;
		updateSpriteTextureRect();
	}
}

size_t AnimatedSprite::addAnimation(size_t nFrames) {
	size_t index = m_animations.size();

	AnimationDescription desc = {};
	desc.nFrames = nFrames;
	desc.animationRowStart = m_nextAnimationRow;

	m_nextAnimationRow += nFrames / m_nTextureColumns + (nFrames % m_nTextureColumns != 0);

	m_animations.push_back(desc);
	if (index == 0) {
		setAnimation(index);
	}
	return index;
}

void AnimatedSprite::setAnimation(size_t animationIndex, float time) {
	m_currentAnimationTime = time;
	m_currentAnimationFrame = time * m_animationSpeed;
	m_currentAnimationIndex = animationIndex;
	m_currentAnimation = m_animations[animationIndex];

	updateSpriteTextureRect();
}

void AnimatedSprite::setTexture(const sf::Texture& texture, bool resetRect) {
	sf::Vector2u textureSize = texture.getSize();

	m_textureFrameWidth = textureSize.x / m_nTextureColumns;
	m_textureFrameHeight = textureSize.y / m_nTextureRows;

	Sprite::setTexture(texture, false);
}

void AnimatedSprite::setAnimationSpeed(float speed) {
	m_animationSpeed = speed;
}

void AnimatedSprite::updateSpriteTextureRect() {
	sf::IntRect rect;

	size_t column = m_currentAnimationFrame % m_nTextureColumns;
	size_t row = m_currentAnimation.animationRowStart + m_currentAnimationFrame / m_nTextureColumns;

	rect.left = column * m_textureFrameWidth;
	rect.width = m_textureFrameWidth;
	rect.top = row * m_textureFrameHeight;
	rect.height = m_textureFrameHeight;

	setTextureRect(rect);
}
