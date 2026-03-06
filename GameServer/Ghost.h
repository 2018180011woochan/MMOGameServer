#pragma once
#include "Monster.h"

// 서버용 고스트 상태 (네 유니티 코드랑 똑같이 맞춤!)
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

	// 배회 관련
	float _wanderTimer = 0.f;
	float _wanderInterval = 3.f;
	float _targetPosX = 0.f;
	float _targetPosZ = 0.f;

	// 추격 관련
	uint64 _targetPlayerId = 0; // 어그로 끌린 플레이어 ID
	float _detectionRange = 12.f;

	// 텔레포트 쿨타임 관련
	float _lastTeleportTime = -999.f;
	float _teleportCooldown = 1.5f;

	float _attackTimer = 0.0f;
};
