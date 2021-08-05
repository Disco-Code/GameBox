#pragma once
#include <unordered_map>
#include "SFML/Graphics/Sprite.hpp"
#include "entityx/entityx.h"

struct AnimationComponent {
	float m_animationSpeed = 24.0f; // The speed at which the animation is playing  (value in frames per second).
	/// <summary>
	/// Call to change which animation to play.
	/// </summary>
	/// <param name="index">The animation index for the animation you want to switch to. The animation index for a specific animation is generated when first calling AddAnimation in the AnimatedSheetDescription used to register the sprite sheet.</param>
	/// <param name="time">Start playing the new animation from this point (value in seconds).</param>
	inline void SetAnimationIndex(size_t index, float time = 0);
	/// <summary>
	/// Get the Index of the current animation.
	/// </summary>
	/// <returns>Index</returns>
	inline size_t GetAnimationIndex() const;
	/// <summary>
	/// Set the time of the animation.
	/// </summary>
	/// <param name="time">time in seconds.</param>
	inline void SetAnimationTime(float time);
	/// <summary>
	/// Get the time of the animation.
	/// </summary>
	/// <returns>Time in seconds.</returns>
	inline float GetAnimationTime() const;
	/// <summary>
	/// Set the speed of the Animation.
	/// </summary>
	/// <param name="speed">Number of frames per second.</param>
	inline void SetAnimationSpeed(float speed);
	/// <summary>
	/// Get the speed of the animation.
	/// </summary>
	/// <returns>Number of frames per second.</returns>
	inline float GetAnimationSpeed() const;
	/// <summary>
	/// Set the current frame of the animation.
	/// </summary>
	/// <param name="frame">Frame to display. This values is rounded down when selecting the frame.</param>
	inline void SetAnimationFrame(float frame);
	/// <summary>
	/// Returns the current animation frame.
	/// </summary>
	/// <returns>Current point of the animation. Round this value down to get the current frame index of the current animation.</returns>
	inline float GetAnimationFrame();
	/// <summary>
	/// Progress the time of the animation.
	/// </summary>
	/// <param name="dt">Time in seconds to progress the animation.</param>
	inline void ProgressAnimation(float dt);
private:
	size_t m_currentAnimationIndex = 0; //Stores the current animation index
	float m_currentAnimationFrame = 0.0f; //Stores the current animation frame
};

struct AnimationDescription {
	size_t nFrames;
	size_t animationRowStart;
};

struct AnimatedSheetDescription {
	AnimatedSheetDescription();
	/// <summary>
	/// Construct a AnimatedSheetDescription.
	/// </summary>
	/// <param name="textureSize">Full size of the sprite sheet.</param>
	/// <param name="nColumns">The number of animated frames that can fit horizontally in the sprite sheet.</param>
	/// <param name="nRows">The number of animated frames that can fit vertically in the sprite sheet.</param>
	AnimatedSheetDescription(const sf::Vector2u& textureSize, size_t nColumns, size_t nRows);
	/// <summary>
	/// Register an animation in the sprite sheet.
	/// </summary>
	/// <param name="nFrames">Number of frames the animation has.</param>
	/// <returns>The generated animation index for the new animation. Pass this to AnimationComponent::SetAnimationIndex to make an entity play this animation.</returns>
	size_t AddAnimation(size_t nFrames);
	const AnimationDescription& GetAnimationDescription(size_t index) const;
	/// <summary>
	/// The width of a single animated frame in the sheet.
	/// </summary>
	/// <returns>Width in pixels.</returns>
	inline size_t GetFrameWidth() const;
	/// <summary>
	/// The height of a single animated frame in the sheet.
	/// </summary>
	/// <returns>Height in pixels.</returns>
	inline size_t GetFrameHeight() const;
	/// <summary>
	/// The number of animated frames that can fit horizontally in the sprite sheet.
	/// </summary>
	/// <returns>Number of Columns.</returns>
	inline size_t GetnColumns() const;
	/// <summary>
	/// The number of animated frames that can fit vertically in the sprite sheet.
	/// </summary>
	/// <returns>Number of Rows.</returns>
	inline size_t GetnRows() const;

private:
	std::vector<AnimationDescription> m_animations;
	size_t m_nextAnimationRow = 0;
	size_t m_animationFrameWidth = 0;
	size_t m_animationFrameHeight = 0;
	size_t m_nTextureColumns = 0;
	size_t m_nTextureRows = 0;
};

/// <summary>
/// Used to set up animated sprites.
/// ===How To Use==
/// 1) Find/create a sprite sheet that contains the animated frames.
///		1a) The sprite sheet can contain multiple animations.
///		1b) Animations does not have to have the same amount of frames.
///		1c) Animations are progressed from left to right in the sheet, animations can span multiple rows and will automatically go down one row then reaching the last frame on current the row.
///		1d) Each new animation HAS to start on a new row in the sheet.
/// 2) Register AnimationSystem to the ECS (once).
/// 3) Register the sprite sheet to the animation system using "AddAnimatedSheetDescription".
///		3a) call "addAnimation" in the AnimatedSheetDescription for each unique animation the sprite contain and pass the number of frames each animation have.
///		3b) "addAnimation" should be called for the animations in the same order as they are organized in the sheet from top to bottom.
/// 4) Add both sf::Sprite and AnimationComponent as components to the entity.
///		4a) m_animationSpeed in AnimationComponent can be used to set the speed for the animation per entity if the default speed is not good enough.
///		4b) If the sprite sheet contains multiple animations, call "SetAnimationIndex" in AnimationComponent to switch which animation is playing.
/// 5) call "addAnimation" for each unique animation the sprite contain.
///		5a) addAnimation should be called for the animations in the same order as they are organized in the sheet from top to bottom.
///	6) AnimationSystem has to be updated each frame to progress the animations.
/// </summary>
class AnimationSystem : public entityx::System<AnimationSystem> {
public:
	AnimationSystem();
	~AnimationSystem();

	void update(entityx::EntityManager& es, entityx::EventManager& events, entityx::TimeDelta dt) override;
	/// <summary>
	/// Add register a new sprite sheet that can be used for animation.
	/// </summary>
	/// <param name="texture">A pointer to the Texture containing the sprite sheet.</param>
	/// <param name="desc">A description that describes the animations within the sprite sheet.</param>
	void AddAnimatedSheetDescription(const sf::Texture* texture, const AnimatedSheetDescription& desc);

private:

	const AnimatedSheetDescription* GetAnimatedSheetDescription(const sf::Texture* texture);
	std::unordered_map<const sf::Texture*, AnimatedSheetDescription> m_animatedSheets;
};