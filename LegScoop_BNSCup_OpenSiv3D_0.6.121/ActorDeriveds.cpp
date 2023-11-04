#include "stdafx.h"
#include "ActorDeriveds.h"

extern InputDirector* inputDirector;
extern ComboCounter* comboCounter;
extern GameProperties* ptrGameProperties;

extern Array<Item*> itemList;

extern Array<CritEffect*>* ptrEffectsArray;//エフェクト配列へのポインタ
extern Array<Coin*>* ptrCoinArray;;
extern Array<ActorItem*>* ptrItemArray;//インスタンス配列へのポインタ

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

Player::Player(String _name) : Actor(_name) {

	//歩行速度を初期化
	walkSpeed = 300;

	//キック攻撃速度を初期化
	attackSpeed = 1000;

	//はじき入力で判定される入力終了までの時間を初期化
	repelInputAcceptDuration = 0.03s;

	//キック攻撃状態時間を初期化
	attackingDuration = 0.2s;

	//ダウン状態時間を初期化
	damageDuration = 2.0s;

	//無敵時間を初期化
	invTimeDuration = 1.0s;

	//各種タイマーリセット
	repelInputTimer.set(repelInputAcceptDuration);
	attackingTimer.set(attackingDuration);
	damagedTimer.set(damageDuration);
	invTimeTimer.set(invTimeDuration);

	//初期位置は画面の中心
	pos = Scene::Center();

	//判定を生成　幅100 高さ200
	collision = new Rect(100, 200);

	//キック攻撃判定を生成　幅200 高さ40
	leg = new Rect(200, 40);
}

void Player::Update()
{
	//はじき入力で判定される入力終了までの時間をマウス入力・proコンで別に設定 初期化値は無効
	if (MouseL.pressed())
	{
		repelInputAcceptDuration = 0.1s;

	}
	else
	{
		repelInputAcceptDuration = 0.03s;

	}

	//キック判定はキック攻撃状態以外では画面外に退避
	leg->setPos(0, -1000);

	//方向dirはいろいろ使う
	dir = 0;

	//無敵時間が完了したらくらい判定フラグcanHitを有効化
	if (invTimeTimer.reachedZero())
	{
		invTimeTimer.reset();
		canHit = true;
	}
	else
	{
		//無敵時間が完了していなければくらい判定フラグは常にfalse
		//falseの代入は無敵時間タイマーが開始した後のみ実行される
		if (invTimeTimer.isStarted())
		{
			canHit = false;
		}
	}

	//ダウン時（ステート"damage"時）
	//被ダメージ時に代入された速度で飛び上がり、重力を加算して攻撃を受けた地点高さにもどったら静止する
	//ダウン状態タイマーが完了するまで操作不能
	if (state == U"damage" || state == U"down")
	{
		//アニメ
		texDamaged->ReStartOnStopped();

		vel.y += 900 * Scene::DeltaTime();

		if (pos.y > damagedPos.y)
		{
			vel = Vec2{ 0,0 };

			//アニメ
			texDamaged->Stop();
			texDown->ReStartOnStopped();
			state = U"down";
		}

		if (damagedTimer.reachedZero())
		{
			damagedTimer.reset();
			state = U"default";
			invTimeTimer.restart();

			//アニメ
			texDown->Stop();
		}
	}
	else
	{
		//キック攻撃状態時　（ステート"attack"時）
		//ステートを"attack"に　攻撃速度で等速直線運動	キック攻撃判定を足元に設定
		//キック攻撃状態タイマーが完了したらはじき入力を有効化してステートを"default"に
		if (attackingTimer.reachedZero())
		{
			attackingTimer.reset();
			attackFootHeight = 0;
			canRepel = true;

			repelInputTimer.reset();
			state = U"default";
			//invTimeTimer.restart();
		}

		if (attackingTimer.isRunning())
		{
			//canHit = false;
			state = U"attack";
			vel.x = lastDir * attackSpeed;
			vel.y = 0;
			Point legpos = Point{ 0,(int32)(50 * attackFootHeight) - 50 };
			if (lastDir > 0)
			{
				leg->setPos(Arg::leftCenter(legpos + Point{ (int32)pos.x,(int32)pos.y }));
			}
			else
			{
				leg->setPos(Arg::rightCenter(legpos + Point{ (int32)pos.x,(int32)pos.y }));
			}

			//アニメ
			texAttacking->ReStartOnStopped();

		}
		else
		{
			//それ以外（便宜上ステート"default"時）
			//上下移動・はじき入力の判定
			vel.y = inputDirector->dir.y * walkSpeed;
			vel.x = inputDirector->dir.x * walkSpeed;

			dir = Sign(inputDirector->dir.x);

			if (Abs(inputDirector->dir.x) <= 0.1)
			{
				repelInputTimer.reset();
			}
			if (canRepel)
			{
				if (Abs(inputDirector->dir.x) > 0.1 && Abs(inputDirector->dir.x) < 0.9)
				{
					if (!repelInputTimer.isStarted())
					{
						repelInputTimer.restart();
					}
				}
				if (Abs(inputDirector->dir.x) >= 0.9)
				{
					if (!repelInputTimer.reachedZero() && repelInputTimer.isStarted())
					{
						state = U"attack";

						attackFootHeight = inputDirector->dir.y;
						canRepel = false;
						attackingTimer.restart();
					}
				}
			}

			//アニメ
			texAttacking->Stop();
			if (vel.length() > 0.1)
			{
				texWalking->ReStartOnStopped();
				texStanding->Stop();
			}
			else
			{
				texWalking->Stop();
				texStanding->ReStartOnStopped();
			}
		}
	}

	//Print << state;


	pos += vel * Scene::DeltaTime();

	if (dir != 0)
	{
		lastDir = dir;
	}

	isInvincible = false;
	Actor::Update();
}

void Player::Draw()
{

	//Circle{ pos + Vec2{0,(state == U"damage") ? 0 : -100},40 }.draw(canHit ? Palette::Red : Palette::Pink);
	bool mirror = false;
	mirror = lastDir < 0;
	if (state == U"default")
	{

		if (vel.length() > 0.1)
		{
			texWalking->GetTexture()->mirrored(mirror).draw(Arg::bottomCenter(pos));
		}
		else
		{
			texStanding->GetTexture()->mirrored(mirror).draw(Arg::bottomCenter(pos));
		}
	}
	else if (state == U"attack")
	{
		texAttacking->GetTexture()->mirrored(mirror).draw(Arg::bottomCenter(pos));

	}
	else if (state == U"damage")
	{
		texDamaged->GetTexture()->mirrored(mirror).draw(Arg::bottomCenter(pos));
	}
	else if (state == U"down")
	{
		texDown->GetTexture()->mirrored(mirror).draw(Arg::bottomCenter(pos));
	}
	//leg->drawFrame(2.0, Palette::Hotpink);
	//Actor::Draw();
}

void Player::OnHit()
{
	//くらい判定をキック攻撃状態時のみにフィルタリング
	//この関数がisInvincible = false 以降に実行されると（当たり判定前にUpdateを呼び出すと）無敵時間が無効になる！
	if (canHit && state == U"attack" && !isInvincible)
	{
		state = U"damage";
		damagedPos = pos;
		vel = Vec2{ 100,-200 };
		damagedTimer.restart();
		canHit = false;
	}
}

void Player::OnAttackHit()
{
	invTimeTimer.restart();
}

void Player::OnCollectCoin()
{
	ptrGameProperties->coin += 10000;
}

void Climber::AnimTexSelect::select(AnimatedTextureInstance* _sel)
{
	for (int32 i = 0; i < list.size(); i++)
	{
		if (list.at(i) == _sel)
		{
			list.at(i)->ReStartOnStopped();
		}
		else
		{
			list.at(i)->Stop();
		}
	}
}

Climber::Climber(String _name, Vec2 _pos) : Actor(_name) {

	//移動スピードを初期化　100,200,300　から一つをランダムで選ぶ
	Array<float> walkptn = { 100,200,300 };
	walkSpeed = walkptn.choice();

	//攻撃スピードを初期化（未使用）
	attackSpeed = 1000;

	//位置を初期化　コンストラクタ引数からもらう
	pos = _pos;

	//判定を生成
	collision = new Rect(100, 200);
	//判定の位置を合わせる
	collision->setPos(Arg::bottomCenter((int32)pos.x, (int32)pos.y));

	//くらい判定を生成
	leg = new Rect(100, 200);
	//判定の位置を合わせる
	leg->setPos(Arg::bottomCenter((int32)pos.x, (int32)pos.y));

	//速度を設定　角度と移動スピードから設定
	vel = OffsetCircular{ Vec2{0,0},walkSpeed,70_deg };




	//ダウン状態時間を初期化
	damageDuration = 1.0s;

	//攻撃状態時間を初期化
	attackTimer.set(0.5s);
	attackTimer.reset();

	//無敵時間を初期化
	invTimeDuration = 0.1s;

	//各種タイマーリセット
	damagedTimer.set(damageDuration);
	invTimeTimer.set(invTimeDuration);

	animCurrent = texStanding;

}

void Climber::Update()
{
	animSelect.list.clear();

	animSelect.list << texStanding;
	animSelect.list << texWalking;
	animSelect.list << texAttacking;
	animSelect.list << texDamaged;
	animSelect.list << texDown;
	animSelect.list << texStandUp;
	animSelect.list << texDefeat;


	//Print << animSelect.list;


	//デバッグ全滅攻撃 Shift + 左クリック
	if (KeyShift.pressed() && MouseL.down())
	{
		//OnCollsitionLeg();
	}
	//ステートが"defeated"の時は重力に従う
	//無条件で位置posに速度velを足す
	//画面外に出たら削除済みフラグisDestroyedを立てる
	if (state == U"defeated")
	{
		vel.y += 900 * Scene::DeltaTime();
		vel.x -= 500 * Scene::DeltaTime();

		rotation -= 3 * Scene::DeltaTime();
		mitame = collision->rotated(rotation);

		//アニメ
		animSelect.select(texDefeat);
		animCurrent = texDefeat;
	}

	pos += vel * Scene::DeltaTime();

	if (!collision->intersects(Rect{ -1000,0,4000,2000 }))
	{
		isDestroyed = true;
	}

	//方向dirはいろいろ使う
	dir = 0;

	//無敵時間が完了したらくらい判定フラグcanHitを有効化
	if (invTimeTimer.reachedZero())
	{
		invTimeTimer.reset();
		canHit = true;
	}
	else
	{
		//無敵時間が完了していなければくらい判定フラグは常にfalse
		//falseの代入は無敵時間タイマーが開始した後のみ実行される
		if (invTimeTimer.isStarted())
		{
			canHit = false;
		}
	}

	//ダウン時（ステート"damage"時）
	//被ダメージ時に代入された速度で飛び上がり、重力を加算して攻撃を受けた地点高さにもどったら静止する
	//ダウン状態タイマーが完了するまで操作不能
	if (state == U"damage")
	{
		vel.y += 900 * Scene::DeltaTime();

		rotation -= 3 * Scene::DeltaTime();
		mitame = collision->rotated(rotation);

		//アニメ
		animSelect.select(texDamaged);
		animCurrent = texDamaged;

		if (pos.y > damagedPos.y)
		{
			vel = Vec2{ 0,0 };
			mitame = collision->rotated(90_deg);


			//アニメ
			animSelect.select(texDown);
			animCurrent = texDown;
		}

		if (damagedTimer.remaining() < 0.5s)
		{
			//アニメ
			animSelect.select(texStandUp);
			animCurrent = texStandUp;
		}

		if (damagedTimer.reachedZero())
		{
			damagedTimer.reset();
			state = U"default";
			invTimeTimer.restart();

			//速度を設定　角度と移動スピードから設定
			vel = OffsetCircular{ Vec2{0,0},walkSpeed,70_deg };
			rotation = 0;
		}

	}

	if (state == U"attack")
	{

		//アニメ
		animSelect.select(texAttacking);
		animCurrent = texAttacking;


		if (attackTimer.reachedZero())
		{
			attackTimer.reset();
			state = U"default";

			//速度を設定　角度と移動スピードから設定
			vel = OffsetCircular{ Vec2{0,0},walkSpeed,70_deg };
			rotation = 0;
		}
	}

	if (state == U"default")
	{


		if (vel.length() > 0.1)
		{

			//アニメ
			animSelect.select(texWalking);
			animCurrent = texWalking;
		}
		else
		{

			//アニメ
			animSelect.select(texStanding);
			animCurrent = texStanding;
		}
	}



	//食らい判定の位置更新
	leg->setPos(Arg::bottomCenter((int32)pos.x, (int32)pos.y));

	Actor::Update();
}

void Climber::Draw()
{
	if (state == U"default")
	{
		//Actor::Draw();
	}
	else
	{
		//mitame.drawFrame(2, Palette::Pink);
	}
	//leg->drawFrame(3, Palette::Hotpink);

	animCurrent->GetTexture()->mirrored(false).draw(Arg::bottomCenter(pos));

}

void Climber::Attack()
{
	if (state != U"attack")
	{
		state = U"attack";
		attackTimer.restart();
	}
}

void Climber::OnCollsitionLeg(Rect _leg)
{
	//コンボ判定は敵側で行う
	//canhit
	if (canHit)
	{
		//ヘルス計算
		int32 damage = 1;
		if (RandomBool(
			Math::Map(Abs(leg->center().y - _leg.center().y) / ptrGameProperties->critChance,
				1, 100, 0, 1)))
		{
			*ptrEffectsArray << new CritEffect(U"crit", leg->center());
			damage = 5;
		}
		hp -= damage;

		if (hp < 0)
		{
			state = U"defeated";
			vel = Vec2{ 0,0 };
			canHit = false;

			rotation = 0;
			mitame = collision->rotated(0);

			comboCounter->Hit();

			//コイン生成
			int32 coinRate = 2;
			coinRate += ptrGameProperties->coinPlus;
			coinRate *= ptrGameProperties->coinMultiply;
			for (int32 i = 0; i < coinRate; i++)
			{
				GenereteCoin();
			}

			if (RandomBool(ptrGameProperties->itemChance))
			{
				GenereteItem();
			}

		}
		else
		{
			state = U"damage";
			damagedPos = pos;
			vel = Vec2{ -100,-200 };
			damagedTimer.restart();
			canHit = false;

			rotation = 0;
			mitame = collision->rotated(0);
			comboCounter->Hit();

			//コイン生成
			int32 coinRate = 1;
			coinRate += ptrGameProperties->coinPlus;
			coinRate *= ptrGameProperties->coinMultiply;
			for (int32 i = 0; i < coinRate; i++)
			{
				GenereteCoin();

			}
		}
	}
}

void Climber::GenereteCoin()
{
	Coin* item = new Coin(U"item", pos);
	*ptrCoinArray << item;
}

void Climber::GenereteItem()
{
	ActorItem* item = new ActorItem(U"item", pos);
	*ptrItemArray << item;
}
