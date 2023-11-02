﻿#include "stdafx.h"
#include "ItemDeriveds.h"
#include "GameProperties.h"
#include "ComboCounter.h"
#include "ActorDeriveds.h"

extern ComboCounter* comboCounter;
extern GameProperties* ptrGameProperties;
extern Player* ptrPlayer;

ItemStatue::ItemStatue() : Item()
{
	name = U"ItemStatue";
	splashText = U"黄金の像！登山者の数が増加！";
	duration = 8s;
	color = Palette::Pink;
}

void ItemStatue::Effect()
{
	ptrGameProperties->climberRate += 1;
}

ItemAmulet::ItemAmulet() : Item()
{
	name = U"ItemAmulet";
	splashText = U"金運の護符！ドロップお金が増加！";
	duration = 20s;
	color = Palette::Blue;
}

void ItemAmulet::Effect()
{
	ptrGameProperties->coinPlus += 2;
}

ItemThron::ItemThron()
{
	name = U"ItemThron";
	splashText = U"棘！コンボ倍率増大！";
	duration = 20s;
	color = Palette::Blue;
}

void ItemThron::Effect()
{
	ptrGameProperties->coinMultiply += comboCounter->combo;
}

ItemLetter::ItemLetter()
{
	name = U"ItemLetter";
	splashText = U"贖宥状！無敵！";
	duration = 20s;
	color = Palette::Blue;
}

void ItemLetter::Effect()
{
	ptrPlayer->isInvincible = true;
}

ItemWheel::ItemWheel()
{
	name = U"ItemWheel";
	splashText = U"輪廻の車輪！アイテムドロップ確率増加！";
	duration = 20s;
	color = Palette::Blue;
}

void ItemWheel::Effect()
{
	ptrGameProperties->itemChance = 0.7;
}

ItemScope::ItemScope()
{
	name = U"ItemScope";
	splashText = U"拡大鏡！クリティカル確率増加！";
	duration = 20s;
	color = Palette::Blue;
}

void ItemScope::Effect()
{
	ptrGameProperties->critChance += 0.25;
}
