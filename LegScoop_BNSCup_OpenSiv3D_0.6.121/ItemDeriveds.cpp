#include "stdafx.h"
#include "ItemDeriveds.h"
#include "GameProperties.h"

extern GameProperties* ptrGameProperties;

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
