#ifndef PHYSICSSYSTEM_H
#define PHYSICSSYSTEM_H

// Inherits
#include "System.h"

// Includes
#include "../Components/Components.h"
#include "../Components/PositionComponent.h"
#include "../Components/BodyComponent.h"
#include "../Entities/Entity.h"
#include <Box2D/Box2D.h>

/// <summary>
/// Physics System used to update positions of entities based on their Box2D bodies
/// </summary>
class PhysicsSystem : public System
{
public:
	PhysicsSystem(float worldScale);
	void update();
private:
	float m_worldScale;
};

#endif // !PHYSICSSYSTEM_H