#include "stdafx.h"
#include "ActorDeriveds.h"
#include "GameProperties.h"

extern GameProperties* ptrGameProperties;
extern Array<Item*> itemList;

CritEffect::CritEffect(String _name, Vec2 _pos) : Actor(_name)
{
	font = ptrGameProperties->fontDefault;

	vel = Vec2{ 0,-100 };
	pos = _pos;

	lifetime.set(1s);
	lifetime.start();
}

void CritEffect::Update()
{
	pos += vel * Scene::DeltaTime();

	if (lifetime.reachedZero())
	{
		isDestroyed = true;
	}
}

void CritEffect::Draw()
{
	font(U"Crit!").drawAt(Math::Lerp(20, 15, Math::Sin(Scene::Time() * 3)), pos, ColorF(Palette::Yellow, Math::Map(lifetime.progress1_0(), 0.0, 0.5, 0.0, 1.0)));
}

Coin::Coin(String _name, Vec2 _pos) : Actor(_name) {

	vel = Vec2{ Random(-100,100),-200 };

	state = U"jump";
	//位置を初期化　コンストラクタ引数からもらう
	pos = _pos;

	startPos = _pos;

	//判定を生成
	collision = new Rect(50, 50);
	//判定の位置を合わせる
	collision->setPos(Arg::bottomCenter((int32)pos.x, (int32)pos.y));

	sucPhase.set(1s);


}

void Coin::Update()
{
	pos += vel * Scene::DeltaTime();

	//生成ジャンプ時（ステート"jump"時）
	//生成時に代入された速度で飛び上がり、重力を加算して攻撃を受けた地点高さにもどったら静止する
	if (state == U"jump")
	{
		canHit = false;
		vel.y += 900 * Scene::DeltaTime();


		if (pos.y > startPos.y)
		{
			vel = Vec2{ 0,0 };

			state = U"default";
		}
	}

	if (state == U"default")
	{
		vel = Vec2{ 0,0 };
		canHit = true;
	}

	if (state == U"suction")
	{
		Vec2 relate = pos - *sucTarget;
		pos = pos.lerp(*sucTarget, sucPhase.progress0_1());
		if (relate.length() < 10)
		{
			isDestroyed = true;
		}
	}

	Actor::Update();
}

void Coin::Draw()
{
	Actor::Draw();

}

void Coin::OnCollision(Vec2* _targetpos, void(*_onCollide)(void))
{
	if (state == U"default")
	{
		//_player->OnCollectCoin();
		//Print << U"collectcoin";
		_onCollide();

		sucTarget = _targetpos;
		state = U"suction";
		sucPhase.start();
	}
}

ActorItem::ActorItem(String _name, Vec2 _pos) : Actor(_name) {
	//アイテムをランダムに選択
	item = itemList.choice();

	vel = Vec2{ Random(-100,100),-200 };

	state = U"jump";
	//位置を初期化　コンストラクタ引数からもらう
	pos = _pos;

	startPos = _pos;

	//判定を生成
	collision = new Rect(50, 50);
	//判定の位置を合わせる
	collision->setPos(Arg::bottomCenter((int32)pos.x, (int32)pos.y));

	sucPhase.set(1s);


}

void ActorItem::Update()
{
	pos += vel * Scene::DeltaTime();

	//生成ジャンプ時（ステート"jump"時）
	//生成時に代入された速度で飛び上がり、重力を加算して攻撃を受けた地点高さにもどったら静止する
	if (state == U"jump")
	{
		canHit = false;
		vel.y += 900 * Scene::DeltaTime();


		if (pos.y > startPos.y)
		{
			vel = Vec2{ 0,0 };

			state = U"default";
		}
	}

	if (state == U"default")
	{
		vel = Vec2{ 0,0 };
		canHit = true;
	}

	if (state == U"suction")
	{
		Vec2 relate = pos - *sucTarget;
		pos = pos.lerp(*sucTarget, sucPhase.progress0_1());
		if (relate.length() < 10)
		{
			isDestroyed = true;
			onGet(item);

		}
	}

	Actor::Update();
}

void ActorItem::Draw()
{
	Actor::Draw();
	Circle{ pos, 25 }.draw(item->color);
}

void ActorItem::OnCollision(Vec2* _targetpos, void(*_onCollide)(Item* item))
{
	if (state == U"default")
	{
		//_player->OnCollectCoin();
		//Print << U"collectcoin";
		onGet = _onCollide;

		sucTarget = _targetpos;
		state = U"suction";
		sucPhase.start();
	}
}
