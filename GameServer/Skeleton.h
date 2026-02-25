#pragma once
#include "Monster.h"

class Skeleton : public Monster
{
private:
	float wanderRadius;
	float wanderInterval;
	float attackDelay;

public:
	Skeleton();
	virtual ~Skeleton();

protected:
	virtual void ProcessIdle(float deltaTime) override;
	virtual void ProcessWander(float deltaTime) override;
	virtual void ProcessChase(float deltaTime) override;
	virtual void ProcessAttack(float deltaTime) override;
};