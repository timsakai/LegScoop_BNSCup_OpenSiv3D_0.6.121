#pragma once

class Actor
{

public:
	Vec2 pos;
	Rect* collision;
	String name;
	bool isDestroyed;

	Actor(String _name);
	virtual void Update();
	virtual void Draw();

private:

};
