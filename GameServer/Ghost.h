#pragma once
#include "Monster.h"

class Ghost : public Monster
{
public:
	Ghost();
	virtual ~Ghost();

	virtual void OnDamaged(float damage) override;
};