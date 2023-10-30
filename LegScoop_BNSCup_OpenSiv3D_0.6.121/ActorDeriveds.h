#pragma once
#include "Actor.h"
#include "Item.h"

class CritEffect : public Actor
{
public:
	Vec2 vel;
	Font font;
	Timer lifetime;

	CritEffect(String _name, Vec2 _pos);

	void Update() override;

	void Draw() override;
};

class Player;

class Coin : public Actor
{

public:

	Vec2 vel;

	Coin(String _name, Vec2 _pos);
	virtual void Update() override;
	virtual void Draw() override;

	void OnCollision(Vec2* _targetpos, void(*_onCollide)(void));


private:
	Vec2 startPos;		//生成地点
	Vec2* sucTarget;
	Timer sucPhase;

	bool canHit = true;		//くらい判定　あり・なし　フラグ

	String state = U"default";		//ステート
};

class ActorItem : public Actor
{

public:
	Item* item;
	Vec2 vel;
	void(*onGet)(Item* item);

	ActorItem(String _name, Vec2 _pos);
	virtual void Update() override;
	virtual void Draw() override;

	void OnCollision(Vec2* _targetpos, void(*_onCollide)(Item* item));


private:
	Vec2 startPos;		//生成地点
	Vec2* sucTarget;
	Timer sucPhase;

	bool canHit = true;		//くらい判定　あり・なし　フラグ

	String state = U"default";		//ステート
};
