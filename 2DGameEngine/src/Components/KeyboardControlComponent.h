#ifndef KEYBOARDCOMPONENT_H
#define KEYBOARDCOMPONENT_H

#include <glm/glm.hpp>

struct KeyboardControlComponent
{
	glm::vec2 upVelocity;
	glm::vec2 rightVelocity;
	glm::vec2 downVelocity;
	glm::vec2 leftVelocity;

	KeyboardControlComponent(glm::vec2 upVelocity = glm::vec2(0),
							 glm::vec2 rightVelocity = glm::vec2(0),
							 glm::vec2 downVelocity = glm::vec2(0),
							 glm::vec2 leftVelocity = glm::vec2(0)) 
	{
		this->upVelocity = upVelocity;
		this->rightVelocity = rightVelocity;
		this->downVelocity = downVelocity;
		this->leftVelocity = leftVelocity;
	}
};


#endif // !KEYBOARDCOMPONENT_H
