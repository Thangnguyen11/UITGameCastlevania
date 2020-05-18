#include "Skeleton.h"

Skeleton::Skeleton(float posX, float posY, LPGAMEENTITY target)
{
	this->SetAnimationSet(CAnimationSets::GetInstance()->Get(ANIMATION_SET_SKELETON));
	tag = EntityType::SKELETON;	

	this->posX = posX;
	this->posY = posY;
	this->target = target;

	SetState(SKELETON_STATE_ACTIVE);

	health = 3;
	isDead = false;

	targetDetected = false;
	activated = false;
	targetSwitchDirection = false;
	isJumping = false;
	triggerJump = false;

	mainWeapon = new Bone();
	triggerResetDelay = false;
}

Skeleton::~Skeleton() {}

void Skeleton::Update(DWORD dt, vector<LPGAMEENTITY> *coObjects)
{
	if (health <= 0 || posX < 0 || posX > SCREEN_WIDTH * 3 || posY > 450)
	{
		SetState(SKELETON_STATE_DIE);
		return;
	}

	Entity::Update(dt);

	vY += SKELETON_GRAVITY * dt;

	if (!triggerResetDelay)
	{
		mainWeapon->ResetDelay();
		triggerResetDelay = true;
	}

#pragma region Collision Logic
	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;
	vector<LPGAMEENTITY> bricks;

	coEvents.clear();
	bricks.clear();
	for (UINT i = 0; i < coObjects->size(); i++)
		if (coObjects->at(i)->GetType() == EntityType::BRICK ||
			coObjects->at(i)->GetType() == EntityType::BREAKABLEBRICK)
			bricks.push_back(coObjects->at(i));

	// turn off collision when die 
	if (state != SKELETON_STATE_DIE)
		CalcPotentialCollisions(&bricks, coEvents);

	// No collision occured, proceed normally
	if (coEvents.size() == 0)
	{
		posX += dx;
		posY += dy;
	}
	else
	{
		float min_tx, min_ty, nx = 0, ny;

		FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny);

		// block 
		posX += min_tx * dx + nx * 0.1f;
		posY += min_ty * dy + ny * 0.1f;

		if (nx != 0 && ny == 0)
		{
			if (target->GetPosX() < posX && direction == 1 ||
				target->GetPosX() > posX && direction == -1)		//Wrong Chase-Direction
				TurnAround();
		}
		else
			if (ny == -1)
			{
				vY = 0.1f;
				dy = vY * dt;

				if (isJumping)
				{
					isJumping = false;
					jumpingTimer->AddToTimer(SKELETON_JUMP_TIME);	//Day timer den IsTimeUp va end Jump
				}
			}
	}

	for (UINT i = 0; i < coEvents.size(); i++)
		delete coEvents[i];
#pragma endregion
#pragma region Activate Logic
	if (!activated)
	{
		vX = 0;
	}
	else
	{
		if(!isJumping)
			vX = SKELETON_WALKING_SPEED * direction;
#pragma region Jump Logic
		if (!isJumping && !triggerJump)
		{
			waitingJumpTimer->Start();
			triggerJump = true;
		}
		if (triggerJump && waitingJumpTimer->IsTimeUp())
		{
			jumpingTimer->Start();
			waitingJumpTimer->Reset();
			triggerJump = false;
			randomJump = rand() % 100;				//////////????????????
		}
		if (!jumpingTimer->IsTimeUp())
		{
			Attack();
			if (randomJump <= 50)
				Jump();
			else
				JumpBack();
		}
		else
		{
			jumpingTimer->Reset();
			isJumping = false;
		}
#pragma endregion
	}

	if (!activated)
	{
		if (target != NULL)
		{
			if (GetDistance(D3DXVECTOR2(this->posX, this->posY), D3DXVECTOR2(target->GetPosX(), target->GetPosY())) <= 300 && target->GetState() != 0) //Ngan xac simon kich hoat 
			{
				if (!targetDetected)
				{
					readyTimer->Start();
					targetDetected = true;
				}
			}
		}
	}

	if (targetDetected && readyTimer->IsTimeUp())
	{
		readyTimer->Reset();
		targetDetected = false;
		activated = true;
		FirstJump();
	}

#pragma endregion
#pragma region Switch Direction React
	if (target != NULL)
	{
		if (target->GetPosX() < posX && direction == 1 ||
			target->GetPosX() > posX && direction == -1)		//Wrong Chase-Direction
		{
			if (!targetSwitchDirection)
			{
				reactTimer->Start();
				targetSwitchDirection = true;
			}
		}
	}

	if (targetSwitchDirection && reactTimer->IsTimeUp())
	{
		if (target->GetPosX() < posX) direction = -1;
		else direction = 1;
		reactTimer->Reset();
		targetSwitchDirection = false;
	}
#pragma endregion

	if (!mainWeapon->GetIsDone())
	{
		if (!mainWeapon->GetIsReceivedPos())
		{
			mainWeapon->SetPosition(posX, posY);
			mainWeapon->SetIsReceivedPos(true);			//Chi nhan pos 1 lan sau khi het delay
		}
		mainWeapon->Update(dt, coObjects);
	}
}

void Skeleton::TurnAround()
{
	direction *= -1;
	vX *= -1;
}

void Skeleton::Jump()
{
	isJumping = true;
	vX = SKELETON_WALKING_SPEED * direction;
	vY = -SKELETON_JUMP_SPEED_Y;
}

void Skeleton::JumpBack()
{
	isJumping = true;
	vX = -SKELETON_WALKING_SPEED * direction;
	vY = -SKELETON_JUMP_SPEED_Y;
}

void Skeleton::FirstJump()
{
	isJumping = true;
	vX = SKELETON_WALKING_SPEED * direction;
	vY = -SKELETON_FIRST_JUMP_SPEED_Y;
}

void Skeleton::Attack()
{
	if (mainWeapon->GetIsDone())
	{
		mainWeapon->Attack(posX, direction);
	}
}

void Skeleton::Render()
{
	if (isDead)
		return;

	animationSet->at(0)->Render(direction, posX, posY);

	RenderBoundingBox();

	if (!mainWeapon->GetIsDone())
	{
		mainWeapon->Render();
	}
}

void Skeleton::SetState(int state)
{
	Entity::SetState(state);
	switch (state)
	{
	case SKELETON_STATE_DIE:
		vX = 0;
		vY = 0;
		isDead = true;
		break;
	}
}

void Skeleton::GetBoundingBox(float &l, float &t, float &r, float &b)
{
	//not clean
	if (!isDead)
	{
		l = posX - 15;
		t = posY;
		r = posX + SKELETON_BBOX_WIDTH;
		b = posY + SKELETON_BBOX_HEIGHT;
	}
}