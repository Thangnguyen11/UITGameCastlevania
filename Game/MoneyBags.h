#pragma once
#include "Item.h"

#define BAGS_GRAVITY			0.03f

#define BAGS_DISPLAY_DURATION	4500
#define BAGS_DELAY				600

#define BAG_RED					0
#define BAG_WHITE				1
#define BAG_BLUE				2

class MoneyBags : public Item
{
public:
	MoneyBags(float posX, float posY, EntityType typeBag);
	~MoneyBags();

	void Render();
};

