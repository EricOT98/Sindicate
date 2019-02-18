#include "AiComponent.h"

AiComponent::AiComponent(AiType type, int xMax, int xMin) 
	: m_type(type), 
	m_active(false), 
	m_direction(1),
	m_xMax(xMax),
	m_xMin(xMin)
{
	id = "Ai";
}

AiComponent::~AiComponent()
{
}

void AiComponent::setMovementMarkers(int min, int max)
{
	m_xMin = min;
	m_xMax = max;
}

void AiComponent::setActivationState(bool state)
{
	m_active = state;
}

bool AiComponent::getActivationState()
{
	return m_active;
}

void AiComponent::setNewPosition(VectorAPI pos)
{
	m_changePosition = true;
	m_newPos = pos;
}

VectorAPI AiComponent::getPosition()
{
	m_changePosition = false;
	return m_newPos;
}

bool AiComponent::getChangePositionStatus()
{
	return m_changePosition;
}

void AiComponent::setDirection(int direction)
{
	m_direction = direction == -1 ? -1 : 1;
}

int AiComponent::getDirection()
{
	return m_direction;
}

int AiComponent::getMinX()
{
	return m_xMin;
}

int AiComponent::getMaxX()
{
	return m_xMax;
}
