#include "AiComponent.h"

AiComponent::AiComponent(AiType type, int xMax, int xMin) 
	: m_type(type), 
	m_active(false), 
	direction(1),
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
	m_setPosition = true;
	m_newPos = pos;
}

VectorAPI AiComponent::getPosition()
{
	m_setPosition = false;
	return m_newPos;
}