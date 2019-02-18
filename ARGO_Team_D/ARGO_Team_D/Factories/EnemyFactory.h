#ifndef ENEMYFACTORY_H
#define ENEMYFACTORY_H

#include "..//ECS/Entities/Entity.h"
#include "..//Resource Manager/ResourceManager.h"
#include "..//ECS/Components/PositionComponent.h"
#include "..//ECS/Components/SpriteComponent.h"
#include "..//ECS/Components/BodyComponent.h"
#include "..//ECS/Components/AnimationComponent.h"
#include "..//ECS/Components/AiComponent.h"
#include "..//Client/Client.h"

struct EnemyData
{
	Entity * entity = nullptr;
	PositionComponent * position = nullptr;
	BodyComponent * body = nullptr;
	SpriteComponent * sprite = nullptr;
	AnimationComponent * animation = nullptr;
	AiComponent * ai = nullptr;
};

class EnemyFactory
{
public:
	// Public Functions
	EnemyFactory(ResourceManager * rm, b2World & world, const float SCALE);
	~EnemyFactory();
	EnemyData * createGunEnemy();
	EnemyData * createFlyEnemy();
	EnemyData * createBigEnemy();

private:
	// Private Functions
	EnemyData * createEnemy(string spriteId, int idleFrames, int walkingFrames, int width, int height);

	// Private Members
	ResourceManager * m_resourceManager;
	b2World & m_refWorld;
	const float WORLD_SCALE;
};

#endif // !ENEMYFACTORY_H