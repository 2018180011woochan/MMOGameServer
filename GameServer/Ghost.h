#pragma once
#include "Monster.h"

enum class GhostState
{
	WANDER,
	CHASE,
	DEAD
};

class Ghost : public Monster
{
public:
	Ghost();
	virtual ~Ghost();

	virtual void Update(float deltaTime) override;

	void OnDamaged(float damage);

private:
	GhostState _ghostState = GhostState::WANDER;

	float _wanderTimer = 0.f;
	float _wanderInterval = 3.f;
	float _targetPosX = 0.f;
	float _targetPosZ = 0.f;

	uint64 _targetPlayerId = 0;
	float _detectionRange = 12.f;

	float _attackTimer = 0.0f;

	// 텔레포트 관련 타이머 변수
	float _teleportCooldownTimer = 0.0f;
	float _teleportDelayTimer = 0.0f;
	bool _isTeleportPending = false;
};
