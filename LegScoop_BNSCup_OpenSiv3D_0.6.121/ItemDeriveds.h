#pragma once
#include "Item.h"

class ItemStatue : public Item //敵の数増大
{
public:
	ItemStatue();

	void Effect() override;
};

class ItemAmulet : public Item //ドロップお金増大
{
public:
	ItemAmulet();

	void Effect() override;
};

class ItemThron : public Item //コンボお金倍率増大
{
public:
	ItemThron();

	void Effect() override;
};

class ItemLetter : public Item //無敵
{
public:
	ItemLetter();

	void Effect() override;
};

class ItemWheel : public Item //アイテムのドロップ増大
{
public:
	ItemWheel();

	void Effect() override;
};

class ItemScope : public Item //クリティカル率増大
{
public:
	ItemScope();

	void Effect() override;
};
