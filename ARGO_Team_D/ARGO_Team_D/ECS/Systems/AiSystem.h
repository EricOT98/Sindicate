#ifndef AISYSTEM_H
#define AISYSTEM_H

// Inherits
#include "System.h"

// Includes
#include "../ECS/Components/AiComponent.h"
#include "../ECS/Components/BodyComponent.h"
#include "../ECS/Components/AnimationComponent.h"
#include "../ECS/Components/SpriteComponent.h"
#include "../Bullets/BulletManager.h"

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
	// Public Functions
	AiSystem(BulletManager * bulletManager, BodyComponent * playerBody, const float SCALE);
	~AiSystem();
	void addEntity(Entity * e) override;
	void update();
	void removeEntity(const int id) override;
private:
	// Private Functions
	void handleGroundEnemy(AiComponents & ac);
	void handleFlyEnemy(AiComponents & ac);

	// Private Members
	BulletManager * m_bulletManager;
	BodyComponent * m_playerBody;
	const float WORLD_SCALE;
	std::vector<string> m_allowedTypes;
	std::map<int, AiComponents> m_components;
	const float DISTANCE_THRESHOLD;
};

#endif // !AISYSTEM_H
