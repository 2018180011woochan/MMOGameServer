#pragma once
#include "Monster.h"

class Golem : public Monster
{
private:
	float wanderRadius;
	float wanderInterval;
	float attackDelay; 

	float attackCooldown;
	float currentCooldown;
	float chaseSpeed;
	float wanderSpeed;
public:
	Golem();
	virtual ~Golem();

protected:
	virtual void ProcessIdle(float deltaTime) override;
	virtual void ProcessWander(float deltaTime) override;
	virtual void ProcessChase(float deltaTime) override;
	virtual void ProcessAttack(float deltaTime) override;

	virtual void Update(float deltaTime) override;
};