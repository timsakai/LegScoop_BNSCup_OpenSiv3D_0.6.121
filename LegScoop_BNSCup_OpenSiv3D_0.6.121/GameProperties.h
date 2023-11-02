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

	float itemChance = 0.5; //ToDO
	float critChance = 1.0; //ToDO

	Array<String> splashText;
	Array<ItemInstance*> items;

	int32 climberRate;
	int defeatCount;

	void ApplyItem();

	void Draw();

private:
	int32 coinDisplay;
};
