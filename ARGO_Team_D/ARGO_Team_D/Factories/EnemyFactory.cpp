#include "EnemyFactory.h"

EnemyFactory::EnemyFactory(ResourceManager * rm, b2World & world, const float SCALE)
	: m_resourceManager(rm),
	m_refWorld(world),
	WORLD_SCALE(SCALE)
{
}

EnemyFactory::~EnemyFactory()
{
}

EnemyData * EnemyFactory::createGunEnemy()
{
	EnemyData * enemy = createEnemy("test", 0, 0, 64, 64);
	enemy->ai = new AiComponent(EnemyGun, 0, 0);
	enemy->entity->addComponent(enemy->ai);
	return enemy;
}

EnemyData * EnemyFactory::createFlyEnemy()
{
	EnemyData * enemy = createEnemy("test", 0, 0, 50, 50);
	enemy->ai = new AiComponent(EnemyFly, 0, 0);
	enemy->entity->addComponent(enemy->ai);
	return enemy;
}

EnemyData * EnemyFactory::createBigEnemy()
{
	EnemyData * enemy = createEnemy("test", 0, 0, 120, 120);
	enemy->ai = new AiComponent(EnemyBig, 0, 0);
	enemy->entity->addComponent(enemy->ai);
	return enemy;
}

EnemyData * EnemyFactory::createEnemy(string spriteId, int idleFrames, int walkingFrames, int width, int height)
{
	// Create Enemy
	EnemyData * enemy = new EnemyData();
	enemy->entity = new Entity();

	// Create Components
	enemy->position = new PositionComponent(0, 0);
	enemy->sprite = new SpriteComponent(spriteId, *m_resourceManager, width, height);
	enemy->body = new BodyComponent(0, 0, width, height, m_refWorld, WORLD_SCALE);
	//enemy->animation = new AnimationComponent(); TBI...

	// Add Components
	enemy->entity->addComponent(enemy->position);
	enemy->entity->addComponent(enemy->sprite);
	enemy->entity->addComponent(enemy->body);
	//enemy->entity->addComponent(enemy->animation); TBI...
	return enemy;
}