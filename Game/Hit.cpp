#include "Hit.h"

Hit::Hit(float posX, float posY)
{
	this->SetAnimationSet(CAnimationSets::GetInstance()->Get(ANIMATION_SET_HITEFFECT));
	this->posX = posX;
	this->posY = posY;
	isDone = false;
	animationSet->at(0)->StartAnimation(SPARKLE_DISPLAY * 2);
}

Hit::~Hit(){}

void Hit::Update(DWORD dt)
{
	if (isDone)	//Khong bo vao Effect::Update do khong return duoc Update o day
	{
		return;
	}
	if (animationSet->at(0)->IsRenderOver())
	{
		isDone = true;
	}
}
