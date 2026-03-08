#pragma once

class Monster : public std::enable_shared_from_this<Monster>
{
public:
	Monster();
	virtual ~Monster();

	int32 monsterId;
	MonsterType type = MONSTER_TYPE_NONE;
	int32 roomId;
	float posX = 0.f, posY = 0.f, posZ = 0.f;
	float destX = 0.f, destY = 0.f, destZ = 0.f;

	float detectionRange = 10.f;
	float attackRange = 2.f;
	float moveSpeed = 2.f;

	float chaseSpeed = 2.f;
	float wanderSpeed = 2.f;
	float wanderRadius = 5.f;
	float wanderInterval = 3.f;
	float attackDelay = 1.f;
	float attackCooldown = 0.f;
	float currentCooldown = 0.f;

	float hp = 100.f;
	float maxHp = 100.f;

	MONSTER_STATE state = STATE_IDLE;
	int32 targetPlayerId = -1;
	float stateTimer = 0.0f;

	weak_ptr<Room> room;

	virtual void Update(float deltaTime);
	virtual void OnDamaged(float damage);

protected:
	virtual void ProcessIdle(float deltaTime);
	virtual void ProcessChase(float deltaTime);
	virtual void ProcessAttack(float deltaTime);
	virtual void ProcessDead(float deltaTime);
	virtual void ProcessWander(float deltaTime);

public:
	virtual void ChangeState(MONSTER_STATE newState);
};