#pragma once
/////TODO: Remove this file if going forward with the ECS solution

#include "gameObjects/gameObject.h"
#include "SFML/Graphics/Texture.hpp"

#include <unordered_map>

struct AnimationDescription {
	size_t nFrames;
	size_t animationRowStart;
};

/// <summary>
/// Used to set up animated Game Objects.
/// ===How To Use==
/// 1) Find/create a sprite sheet that contains the animated frames.
///		1a) The sprite sheet can contain multiple animations.
///		1b) Animations does not have to have the same amount of frames.
///		1c) Animations are updated from left to right in the sheet, animations can span multiple rows and will automatically go down one row then reaching the last frame on current the row.
///		1d) Each new animation HAS to start on a new row in the sheet.
/// 2) Create a AnimatedGameObject.
///		2a) Constructor need to know the number of rows and columns of the sheet that will be used.
/// 3) call "setTexture" to set the sheet.
/// 4) call "addAnimation" for each unique animation the sprite contain.
///		4a) addAnimation should be called for the animations in the same order as they are organized in the sheet from top to bottom.
///	5) Call update each frame to progress the animation
/// 6) Call "setAnimation" set select which animation to run.
/// 7) If the animation is updated to fast or to slow, setAnimationSpeed can be called to modify the animation speed.
///		7a) Input to setAnimationSpeed is "frames per second" (24 is default)
/// </summary>
class AnimatedGameObject : public GameObject {
public:

	/// <summary>
	/// Constructor.
	/// </summary>
	/// <param name="nColumns"></param> Maximum number of columns in the texture that will be used on this object
	/// <param name="nRows"></param> Maximum number of rows in the texture that will used on this object
	AnimatedGameObject(size_t nColumns, size_t nRows);
	~AnimatedGameObject();

	virtual void update(float dt) override;
	/// <summary>
	/// Call this for each unique animation in the sheet from top to bottom. 
	///	Animations does not have to have the same amount of frames.
	///	Animations can span multiple rows.
	///	Each new animation HAS to start on a new row in the sheet.
	/// </summary>
	/// <param name="nFrames">The number of frames that should make up the new animation.</param>
	/// <returns></returns>
	size_t addAnimation(size_t nFrames);
	/// <summary>
	/// Set the animation to run.
	/// </summary>
	/// <param name="animationIndex"></param>
	/// <param name="time">Any animation Index returned by "addAnimation"</param>
	void setAnimation(size_t animationIndex, float time = 0);
	void setTexture(const sf::Texture& texture, bool resetRect = false);
	/// <summary>
	/// If the animation is updated to fast or to slow, setAnimationSpeed can be called to modify the animation speed.
	/// </summary>
	/// <param name="speed">Animation speed in "frames per second". Default is 24.</param>
	void setAnimationSpeed(float speed = 24);

private:
	/// <summary>
	/// Calculates the position in the sheet that contain the current frame of the animation.
	/// </summary>
	void updateSpriteTextureRect();

	size_t m_textureFrameWidth;
	size_t m_textureFrameHeight;

	float m_animationSpeed = 24;

	float m_currentAnimationTime;
	size_t m_currentAnimationFrame;
	size_t m_currentAnimationIndex;
	AnimationDescription m_currentAnimation;
	size_t m_nextAnimationRow;
	size_t m_nTextureColumns;
	size_t m_nTextureRows;

	std::vector < AnimationDescription> m_animations;
};

