#pragma once
#include "Actor.h"
#include "Item.h"
#include "AnimatedTexture.h"

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

class Player : public Actor
{

public:
	float dir;
	float lastDir;
	Vec2 vel;
	Rect* leg;

	AnimatedTextureInstance* texStanding;
	AnimatedTextureInstance* texWalking;
	AnimatedTextureInstance* texAttacking;
	AnimatedTextureInstance* texDamaged;
	AnimatedTextureInstance* texDown;

	Player(String _name);
	virtual void Update() override;
	virtual void Draw() override;
	void OnHit();

	void OnAttackHit();

	void OnCollectCoin();
private:

	float walkSpeed;	//移動スピード
	float attackSpeed;	//攻撃スピード

	Duration repelInputAcceptDuration;		//はじき入力判定時間
	Timer repelInputTimer;		//はじき入力判定タイマー

	Duration attackingDuration;		//キック攻撃状態時間
	Timer attackingTimer;		//キック攻撃状態タイマー

	float attackFootHeight = 0.0f;		//蹴り高さ

	bool canRepel = true;		//はじき入力　可能・不可能　フラグ

	Duration damageDuration;		//ダウン時間
	Timer damagedTimer;		//ダウンタイマー
	Vec2 damagedPos;		//攻撃受けた地点

	bool canHit = true;		//くらい判定　あり・なし　フラグ

	Duration invTimeDuration;		//無敵時間長さ
	Timer invTimeTimer;		//無敵時間タイマー

	String state = U"default";		//ステート
};

class Climber : public Actor
{

public:
	float dir;
	float lastDir;


	int hp = 2;

	Vec2 vel;

	Rect* leg;


	float rotation;
	Quad mitame;

	AnimatedTextureInstance* texStanding;
	AnimatedTextureInstance* texWalking;
	AnimatedTextureInstance* texAttacking;
	AnimatedTextureInstance* texDamaged;
	AnimatedTextureInstance* texDown;
	AnimatedTextureInstance* texStandUp;
	AnimatedTextureInstance* texDefeat;

	AnimatedTextureInstance* animCurrent;

	struct AnimTexSelect
	{
		Array<AnimatedTextureInstance*> list;
		void select(AnimatedTextureInstance* _sel);
	};

	AnimTexSelect animSelect;

	Climber(String _name, Vec2 _pos);
	virtual void Update() override;
	virtual void Draw() override;

	void Attack();

	void OnCollsitionLeg(Rect _leg);


private:

	float walkSpeed;//移動スピード
	float attackSpeed;//攻撃スピード（未使用）

	Duration damageDuration;//ダウン時間
	Timer damagedTimer;		//ダウンタイマー
	Vec2 damagedPos;		//攻撃受けた地点

	Timer attackTimer;		//攻撃状態タイマー

	bool canHit = true;		//くらい判定　あり・なし　フラグ

	Duration invTimeDuration;		//無敵時間長さ
	Timer invTimeTimer;		//無敵時間タイマー

	String state = U"default";		//ステート

	void GenereteCoin();
	void GenereteItem();
};
