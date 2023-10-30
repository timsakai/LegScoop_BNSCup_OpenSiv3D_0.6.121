#pragma once

class Item
{
public:
	String name;
	String splashText;
	Texture texture;
	Duration duration;
	Color color;

	virtual void Effect();
};

struct ItemInstance
{
	Item* item;
	Timer timer;
	//bool isDestroyed;
	ItemInstance(Item* _item);
	void Update();

	bool isDestroyed();
};
