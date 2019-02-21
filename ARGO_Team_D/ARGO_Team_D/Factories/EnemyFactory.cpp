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

Enemy * EnemyFactory::createGunEnemy()
{
	Enemy * enemy = createEnemy("test", 0, 0, 64, 64, false);
	enemy->ai = new AiComponent(EnemyGun, 0, 0, 3);
	enemy->entity->addComponent(enemy->ai);
	return enemy;
}

Enemy * EnemyFactory::createFlyEnemy()
{
	Enemy * enemy = createEnemy("test", 0, 0, 50, 50, true);
	enemy->ai = new AiComponent(EnemyFly, 0, 0, 1);
	enemy->entity->addComponent(enemy->ai);
	return enemy;
}

Enemy * EnemyFactory::createBigEnemy()
{
	Enemy * enemy = createEnemy("test", 0, 0, 120, 120, false);
	enemy->ai = new AiComponent(EnemyBig, 0, 0, 10);
	enemy->entity->addComponent(enemy->ai);
	return enemy;
}

Enemy * EnemyFactory::createEnemy(string spriteId, int idleFrames, int walkingFrames, int width, int height, bool flying)
{
	// Create Enemy
	Enemy * enemy = new Enemy();
	enemy->entity = new Entity();

	// Create Components
	enemy->position = new PositionComponent(-1000, -1000);
	enemy->sprite = new SpriteComponent(spriteId, *m_resourceManager, width, height);
	enemy->body = new BodyComponent(-1000, -1000, width, m_refWorld, WORLD_SCALE, "EnemyBody", flying);
	//enemy->animation = new AnimationComponent(); TBI...

	// Add Components
	enemy->entity->addComponent(enemy->position);
	enemy->entity->addComponent(enemy->sprite);
	enemy->entity->addComponent(enemy->body);
	//enemy->entity->addComponent(enemy->animation); TBI...
	return enemy;
}