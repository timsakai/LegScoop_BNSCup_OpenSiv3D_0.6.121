#pragma once
#include "Item.h"

class ItemStatue : public Item
{
public:
	ItemStatue();

	void Effect() override;
};

class ItemAmulet : public Item
{
public:
	ItemAmulet();

	void Effect() override;
};
