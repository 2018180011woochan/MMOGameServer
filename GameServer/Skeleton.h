#pragma once
#include "Monster.h"

class Skeleton : public Monster
{
private:
	float wanderRadius;
	float wanderInterval;

	float confrontDuration;
	float confrontRadius;
	float confrontSpeed;

	float retreatDistance;
	float retreatDuration;

	bool hasConfrontedOnce = false;

public:
	Skeleton();
	virtual ~Skeleton();

protected:
	virtual void ProcessIdle(float deltaTime) override;
	virtual void ProcessWander(float deltaTime) override;
	virtual void ProcessConfront(float deltaTime) override;
	virtual void ProcessRetreat(float deltaTime) override;
	virtual void ProcessChase(float deltaTime) override;


};

