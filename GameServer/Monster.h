#pragma once

class Monster : public std::enable_shared_from_this<Monster>
{
public:
	Monster();
	virtual ~Monster();

	int32 monsterId;
	int32 roomId; 
	float posX = 0.f, posY = 0.f, posZ = 0.f; 

	float detectionRange = 10.f;
	float attackRange = 2.f;
	float moveSpeed = 2.f;

	MONSTER_STATE state = STATE_IDLE;
	int32 targetPlayerId = -1;
	float stateTimer = 0.0f;

	weak_ptr<Room> room;

	virtual void Update(float deltaTime);

protected:
	// 공통 로직
	virtual void ProcessIdle(float deltaTime);
	virtual void ProcessChase(float deltaTime);
	virtual void ProcessAttack(float deltaTime);
	virtual void ProcessDead(float deltaTime);

	// 특수 로직
	virtual void ProcessWander(float deltaTime) {}
	virtual void ProcessConfront(float deltaTime) {}
	virtual void ProcessRetreat(float deltaTime) {}

	virtual void ChangeState(MONSTER_STATE newState);
};



