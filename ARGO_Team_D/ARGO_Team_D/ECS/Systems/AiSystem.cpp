#include "AiSystem.h"

AiSystem::AiSystem(BodyComponent * playerBody) : m_playerBody(playerBody)
{
	m_allowedTypes = { "Body", "Animation", "Ai", "Sprite" };
}

AiSystem::~AiSystem()
{
}

void AiSystem::addEntity(Entity * e)
{
	auto comps = e->getComponentsOfType(m_allowedTypes);
	if (comps.size() >= m_allowedTypes.size() - 1)
	{
		AiComponents aiComp;
		aiComp.body = dynamic_cast<BodyComponent*>(comps["Body"]);
		aiComp.animation = dynamic_cast<AnimationComponent*>(comps["Animation"]);
		aiComp.ai = dynamic_cast<AiComponent*>(comps["Ai"]);
		aiComp.sprite = dynamic_cast<SpriteComponent*>(comps["Sprite"]);
		m_components.push_back(aiComp);
		m_entityList.push_back(e);
	}
}

void AiSystem::update()
{
	for (auto & ac : m_components)
	{
		bool active = ac.ai->getActivationState();
		auto body = ac.body->getBody();
		ac.sprite->setRender(active);
		if (ac.ai->getChangePositionStatus())
		{
			VectorAPI newPos = ac.ai->getPosition();
			body->SetTransform(b2Vec2(newPos.x, newPos.y), body->GetAngle());
		}
		if (active)
		{
			// TBI
		}
		else
		{
			//body->SetLinearVelocity(b2Vec2(0, 0));
			//body->SetTransform(b2Vec2(0, 0), body->GetAngle());
		}
	}
}