#include "stdafx.h"
#include "Item.h"

void Item::Effect()
{
}

ItemInstance::ItemInstance(Item* _item)
{
	item = _item;
	timer.set(item->duration);
	timer.start();
	//isDestroyed = false;
}

void ItemInstance::Update()
{
	Print << timer;
	if (timer.reachedZero())
	{
		//isDestroyed = true;
	}
}

bool ItemInstance::isDestroyed()
{
	return timer.reachedZero();
}
