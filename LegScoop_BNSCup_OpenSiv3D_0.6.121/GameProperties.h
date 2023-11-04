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

	float itemChance = 0.5;
	float critChance = 1.0; 

	Array<String> splashText;
	Array<ItemInstance*> items;

	int32 climberRate;
	int defeatCount;

	void ApplyItem();

	void Update();

	void Draw() const;

private:
	int32 coinDisplay;
};
