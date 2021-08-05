#include "SpriteAnimationSystem.h"
inline void AnimationComponent::SetAnimationIndex(size_t index, float time) {
	m_currentAnimationIndex = index;
	SetAnimationTime(time);
}

inline size_t AnimationComponent::GetAnimationIndex() const {
	return size_t();
}

inline void AnimationComponent::SetAnimationTime(float time) {
	m_currentAnimationFrame = time * m_animationSpeed;
}

inline float AnimationComponent::GetAnimationTime() const {
	return m_currentAnimationFrame / m_animationSpeed;
}

inline void AnimationComponent::SetAnimationSpeed(float speed) {
	m_animationSpeed = speed;
}

inline float AnimationComponent::GetAnimationSpeed() const {
	return m_animationSpeed;
}

inline void AnimationComponent::SetAnimationFrame(float frame) {
	m_currentAnimationFrame = frame;
}

inline float AnimationComponent::GetAnimationFrame() {
	return m_currentAnimationFrame;
}

inline void AnimationComponent::ProgressAnimation(float dt) {
	m_currentAnimationFrame += dt * m_animationSpeed;
}

AnimatedSheetDescription::AnimatedSheetDescription() {
}

AnimatedSheetDescription::AnimatedSheetDescription(const sf::Vector2u& textureSize, size_t nColumns, size_t nRows) {
	m_nTextureColumns = nColumns;
	m_nTextureRows = nRows;

	m_animationFrameWidth = textureSize.x / m_nTextureColumns;
	m_animationFrameHeight = textureSize.y / m_nTextureRows;
}

size_t AnimatedSheetDescription::AddAnimation(size_t nFrames) {
	size_t index = m_animations.size();

	AnimationDescription desc = {};
	desc.nFrames = nFrames;
	desc.animationRowStart = m_nextAnimationRow;

	m_nextAnimationRow += nFrames / m_nTextureColumns + (nFrames % m_nTextureColumns != 0);

	m_animations.push_back(desc);

	return index;
}

const AnimationDescription& AnimatedSheetDescription::GetAnimationDescription(size_t index) const {
	if (index < m_animations.size()) {
		return m_animations.at(index);
	}
	return m_animations.back();
}

inline size_t AnimatedSheetDescription::GetFrameWidth() const {
	return m_animationFrameWidth;
}

inline size_t AnimatedSheetDescription::GetFrameHeight() const {
	return m_animationFrameHeight;
}

inline size_t AnimatedSheetDescription::GetnColumns() const {
	return m_nTextureColumns;
}

inline size_t AnimatedSheetDescription::GetnRows() const {
	return m_nTextureRows;
}

AnimationSystem::AnimationSystem() {
}

AnimationSystem::~AnimationSystem() {
}

void AnimationSystem::update(entityx::EntityManager& es, entityx::EventManager& events, entityx::TimeDelta dt) {
	es.each<AnimationComponent, sf::Sprite>([&](entityx::Entity entity, AnimationComponent& animationData, sf::Sprite& sprite) {

		const AnimatedSheetDescription* animationSheetDesc = GetAnimatedSheetDescription(sprite.getTexture());

		if (animationSheetDesc) {
			const AnimationDescription& animationDescription = animationSheetDesc->GetAnimationDescription(animationData.GetAnimationIndex());

			animationData.ProgressAnimation(dt);
			size_t nextFrame = ((size_t)animationData.GetAnimationFrame()) % animationDescription.nFrames;

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

void AnimationSystem::AddAnimatedSheetDescription(const sf::Texture* texture, const AnimatedSheetDescription& desc) {
	m_animatedSheets[texture] = desc;
}

const AnimatedSheetDescription* AnimationSystem::GetAnimatedSheetDescription(const sf::Texture* texture) {
	if (m_animatedSheets.count(texture)) {
		return &m_animatedSheets[texture];
	} else {
		return nullptr;
	}
}
