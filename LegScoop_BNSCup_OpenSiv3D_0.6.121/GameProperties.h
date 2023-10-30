#pragma once
#include "Item.h"
	
class GameProperties
{
public:
	Font fontDefault;

	int32 coinGoal;
	int32 coin;
	int32 coinPlus;
	float coinMultiply;

	Array<String> splashText;
	Array<ItemInstance*> items;

	int32 climberRate;
	int defeatCount;

	void ApplyItem();

	void Draw();

private:
	int32 coinDisplay;
};
