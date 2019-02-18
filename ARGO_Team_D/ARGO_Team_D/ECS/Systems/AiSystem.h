#ifndef AISYSTEM_H
#define AISYSTEM_H

// Inherits
#include "System.h"

// Includes
#include "../ECS/Components/AiComponent.h"
#include "../ECS/Components/BodyComponent.h"
#include "../ECS/Components/AnimationComponent.h"
#include "../ECS/Components/SpriteComponent.h"

struct AiComponents
{
	AiComponent * ai;
	BodyComponent * body;
	AnimationComponent * animation;
	SpriteComponent * sprite;
};

class AiSystem : public System
{
public:
	AiSystem(BodyComponent * playerBody);
	~AiSystem();
	void addEntity(Entity * e) override;
	void update();
private:
	BodyComponent * m_playerBody;
	std::vector<string> m_allowedTypes;
	std::vector<AiComponents> m_components;
};

#endif // !AISYSTEM_H
