#ifndef CHARACTERFACTORY_H
#define CHARACTERFACTORY_H

#include "..//ECS/Entities/Entity.h"
#include "..//Resource Manager/ResourceManager.h"
#include "..//ECS/Components/PositionComponent.h"
#include "..//ECS/Components/SpriteComponent.h"
#include "..//ECS/Components/BodyComponent.h"

class PlayerFactory
{
public:
	PlayerFactory(std::string spriteId, VectorAPI dimensions, ResourceManager * rm, b2World & world, const float SCALE);
	Entity * create(int entityId, VectorAPI pos);

private:
	ResourceManager * m_resourceManager;
	b2World & m_refWorld;
	const float WORLD_SCALE;
	std::string m_spriteId;
	VectorAPI m_dimensions;
};

#endif
