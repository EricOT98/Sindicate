#ifndef AICOMPONENT_H
#define AICOMPONENT_H

// AiType dictates how AI will be treated
enum AiType
{
	EnemyGun,
	EnemyFly,
	EnemyBig
};

// Inherits
#include "Components.h"

class AiComponent : public Component
{
public:
	// Public Functions
	AiComponent(AiType type, int xMax, int xMin);
	~AiComponent();
	void setMovementMarkers(int min, int max);
	void setActivationState(bool state);
	bool getActivationState();
	void setDirection(int direction);
	int getDirection();
	int getMinX();
	int getMaxX();
private:
	// Private Members
	AiType m_type;
	bool m_active;
	bool m_changePosition;
	VectorAPI m_newPos;
	int m_direction;
	int m_xMax;
	int m_xMin;
};

#endif // !AICOMPONENT_H
