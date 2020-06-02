#pragma once
#include "Entity.h"
#include "Timer.h"

#define HUNCHMAN_WALKING_SPEED_X		0.2f
#define HUNCHMAN_JUMP_SPEED_Y			0.2f
#define HUNCHMAN_HIGHJUMP_SPEED_Y		0.35f
#define HUNCHMAN_GRAVITY				0.002f
#define HUNCHMAN_MAXHEALTH				1
#define HUNCHMAN_SIGHT_DODGE_RANGE		100
#define HUNCHMAN_SIGHT_RANGE			250

#define HUNCHMAN_BBOX_WIDTH				32
#define HUNCHMAN_BBOX_HEIGHT			32

#define HUNCHMAN_STATE_ACTIVE			0
#define HUNCHMAN_STATE_DIE				-1

#define HUNCHMAN_WAITING_DURATION		900
#define HUNCHMAN_REACT_DELAY			450
#define HUNCHMAN_JUMP_DURATION			150
#define HUNCHMAN_JUMP_COOLDOWN			600

class Hunchman : public Entity
{
	LPGAMEENTITY target;
	bool targetDetected;
	bool activated;
	Timer* readyTimer = new Timer(HUNCHMAN_WAITING_DURATION);
	bool targetSwitchDirection;
	Timer* reactTimer = new Timer(HUNCHMAN_REACT_DELAY);	
	//Them 1 khoang delay nho cho phan ung cua hunchman khi player doi huong
	bool isJumping, triggerJump;
	Timer* jumpingTimer = new Timer(HUNCHMAN_JUMP_DURATION);
	Timer* waitingJumpTimer = new Timer(HUNCHMAN_JUMP_COOLDOWN);

public:
	Hunchman(float posX, float posY, LPGAMEENTITY target);
	~Hunchman();
	virtual void Update(DWORD dt, vector<LPGAMEENTITY> *coObjects);
	virtual void Render();
	virtual void GetBoundingBox(float &l, float &t, float &r, float &b);
	virtual void SetState(int state);

	void TurnAround();
	void ShortJump();
	void LongJump();
};

