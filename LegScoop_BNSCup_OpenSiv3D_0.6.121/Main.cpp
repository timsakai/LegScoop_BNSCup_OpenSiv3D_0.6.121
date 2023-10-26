﻿# include <Siv3D.hpp> // Siv3D v0.6.12

class InputDirector	//機能：クリック地点からの相対マウス位置を正規化して出力　inputDirector->dir　でとれる
{
public:
	Vec2 dir;
	int maxDistance;

	InputDirector()
	{
		maxDistance = 100; //マウススティックの大きさ
		PinchStart = Point(0, 0);
	}

	void Update()
	{
		dir = Vec2(0,0);
		if (MouseL.down())
		{
			PinchStart = Cursor::Pos();
		}
		if (MouseL.pressed())
		{
			Point relate = Cursor::Pos() - PinchStart;
			Vec2 relateF = Vec2(relate);
			dir = relateF / maxDistance;
		}
		else
		{
			//proコンのスティック取得
			if (const auto pro = ProController(0))
			{
				dir = pro.LStick();
				//デッドゾーン
				if (dir.length() < 0.1)
				{
					dir = Vec2{ 0,0 };
				}
				//Print << dir.x;
			}
		}
	}

	void Draw()
	{

		if (MouseL.pressed())
		{
			Circle{ PinchStart ,maxDistance }.drawFrame(2.0, Palette::Lightgrey);
			Circle{ Cursor::Pos(), 20 }.drawFrame(5.0, Palette::White);
		}
		if (const auto pro = ProController(0))
		{
			Circle{Scene::Center(),maxDistance }.drawFrame(2.0, Palette::Lightgrey);
			Circle{ Scene::Center() + dir * maxDistance, 20 }.drawFrame(5.0, Palette::White);

		}
	}
private:
	Point PinchStart;

};

//コンボカウントクラス
class ComboCounter
{
public:
	int32 combo = 0;
	Duration comboDuration;
	Timer comboTimer;
	bool isCommboning;
	ComboCounter()
	{
		comboDuration = 1.5s;
		comboTimer.set(comboDuration);
		isCommboning = false;
	}

	void Update()
	{
		isCommboning = !comboTimer.reachedZero() && comboTimer.isRunning();
		if (!isCommboning)
		{
			combo = 0;
		}
	}

	void Hit()
	{
		comboTimer.restart();
		combo++;
	}

	void Draw(Font _font)
	{
		if (isCommboning)
		{
			_font(combo).drawAt(1400, 200);
		}
	}
};

class Item
{
public:
	String name;
	String splashText;
	Texture texture;
	Duration duration;
	Color color;

	virtual void Effect() {};
};

struct ItemInstance
{
	Item* item;
	Timer timer;
	//bool isDestroyed;
	ItemInstance(Item* _item)
	{
		item = _item;
		timer.set(item->duration);
		timer.start();
		//isDestroyed = false;
	}
	void Update()
	{
		Print << timer;
		if (timer.reachedZero())
		{
			//isDestroyed = true;
		}
	}

	bool isDestroyed()
	{
		return timer.reachedZero();
	}
};

class GameProperties
{
public:
	Font fontDefault;

	int32 coinGoal;
	int32 coin;
	int32 coinPlus;
	float coinMultiply;

	Array<String> splashText;
	Array<ItemInstance*> items;

	int32 climberRate;
	int defeatCount;

	void ApplyItem()
	{
		splashText.clear();
		coinPlus = 0;
		coinMultiply = 1;
		climberRate = 1;
		for (int32 i = 0; i < items.size(); i++)
		{
			splashText << items.at(i)->item->splashText;
			items.at(i)->item->Effect();
		}
	}

	void Draw()
	{
		coinDisplay += Sign(coin - coinDisplay) * 1234;
		if (Abs(coin - coinDisplay) <= 1234) coinDisplay = coin;
		fontDefault(U"￥", coinDisplay).drawBaseAt(40, 800, 750, Palette::White);
		fontDefault(U"/", coinGoal).drawBaseAt(20,800,800,Palette::White);
		Point SplashBase = Point{ 800, 700 };
		for (int32 i = 0; i < splashText.size(); i++)
		{
			fontDefault(splashText.at(i)).drawBaseAt(Math::Lerp(30,25,Math::Sin(Scene::Time() * 3)), SplashBase + Point{0,-40} *i, Palette::Yellow);
		}
	}

private:
	int32 coinDisplay;
};

ComboCounter* comboCounter = nullptr;

InputDirector* inputDirector = new InputDirector();

GameProperties* ptrGameProperties = nullptr;

class Actor
{

public:
	Vec2 pos;
	Rect* collision;
	String name;
	bool isDestroyed;

	Actor(String _name) {
		name = _name;
	}
	virtual void Update()
	{
		if (collision == nullptr) return;
		collision->setPos(Arg::bottomCenter((int32)pos.x, (int32)pos.y));	//コリジョン位置を更新
	}
	virtual void Draw()
	{
		if (collision == nullptr) return;
		collision->drawFrame(2.0, Palette::Greenyellow);	//コリジョンを表示
	}

private:

};

class Player;

class Coin : public Actor
{

public:

	Vec2 vel;

	Coin(String _name, Vec2 _pos) : Actor(_name) {

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
	virtual void Update() override
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
	virtual void Draw() override
	{
		Actor::Draw();

	}

	void OnCollision(Vec2* _targetpos, void(*_onCollide)(void))
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


private:
	Vec2 startPos;		//生成地点
	Vec2* sucTarget;
	Timer sucPhase;

	bool canHit = true;		//くらい判定　あり・なし　フラグ

	String state = U"default";		//ステート
};

Array<Coin*>* ptrCoinArray;//インスタンス配列へのポインタ

class ItemStatue : public Item
{
public:
	ItemStatue() : Item()
	{
		name = U"ItemStatue";
		splashText = U"黄金の像！登山者の数が増加！";
		duration = 20s;
		color = Palette::Pink;
	}

	void Effect() override
	{
		ptrGameProperties->climberRate *= 2;
	}
};

class ItemAmulet : public Item
{
public:
	ItemAmulet() : Item()
	{
		name = U"ItemAmulet";
		splashText = U"金運の護符！ドロップお金が増加！";
		duration = 20s;
		color = Palette::Blue;
	}

	void Effect() override
	{
		ptrGameProperties->coinPlus += 2;
	}
};

Array<Item*> itemList;

class ActorItem : public Actor
{

public:
	Item* item;
	Vec2 vel;
	void(*onGet)(Item* item);

	ActorItem(String _name, Vec2 _pos) : Actor(_name) {
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
	virtual void Update() override
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
	virtual void Draw() override
	{
		Actor::Draw();
		Circle{ pos, 25 }.draw(item->color);
	}

	void OnCollision(Vec2* _targetpos, void(*_onCollide)(Item* item))
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


private:
	Vec2 startPos;		//生成地点
	Vec2* sucTarget;
	Timer sucPhase;

	bool canHit = true;		//くらい判定　あり・なし　フラグ

	String state = U"default";		//ステート
};

Array<ActorItem*>* ptrItemArray;//インスタンス配列へのポインタ

class Player : public Actor
{

public:
	float dir;
	float lastDir;
	Vec2 vel;
	Rect* leg;

	Player(String _name) : Actor(_name) {

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
	virtual void Update() override
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
		if (state == U"damage")
		{
			vel.y += 900 * Scene::DeltaTime();

			if (pos.y > damagedPos.y)
			{
				vel = Vec2{ 0,0 };
			}

			if (damagedTimer.reachedZero())
			{
				damagedTimer.reset();
				state = U"default";
				invTimeTimer.restart();
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
							attackFootHeight = inputDirector->dir.y;
							canRepel = false;
							attackingTimer.restart();
						}
					}
				}

			}
		}
		

		pos += vel * Scene::DeltaTime();

		if (dir != 0)
		{
			lastDir = dir;
		}
		Actor::Update();
	}
	virtual void Draw() override
	{
		Circle{ pos + Vec2{0,(state == U"damage") ? 0 : -100},40}.draw(canHit ? Palette::Red : Palette::Pink);
		leg->drawFrame(2.0, Palette::Hotpink);
		Actor::Draw();
	}

	void OnHit()
	{
		//くらい判定をキック攻撃状態時のみにフィルタリング
		if (canHit && state == U"attack")
		{
			state = U"damage";
			damagedPos = pos;
			vel = Vec2{ 100,-200 };
			damagedTimer.restart();
			canHit = false;
		}
	}

	void OnAttackHit()
	{
		invTimeTimer.restart();
	}

	void OnCollectCoin()
	{
		ptrGameProperties->coin += 10000;
	}
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

Player* ptrPlayer;

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

	Climber(String _name,Vec2 _pos) : Actor(_name) {

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
		leg = new Rect(100, 100);
		//判定の位置を合わせる
		leg->setPos(Arg::bottomCenter((int32)pos.x, (int32)pos.y));

		//速度を設定　角度と移動スピードから設定
		vel = OffsetCircular{ Vec2{0,0},walkSpeed,70_deg };




		//ダウン状態時間を初期化
		damageDuration = 1.0s;

		//無敵時間を初期化
		invTimeDuration = 0.1s;

		//各種タイマーリセット
		damagedTimer.set(damageDuration);
		invTimeTimer.set(invTimeDuration);




	}
	virtual void Update() override
	{
		//デバッグ全滅攻撃 Shift + 左クリック
		if (KeyShift.pressed() && MouseL.down())
		{
			OnCollsitionLeg();
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

		if (pos.y > damagedPos.y)
		{
			vel = Vec2{ 0,0 };
			mitame = collision->rotated(90_deg);
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





		//食らい判定の位置更新
		leg->setPos(Arg::bottomCenter((int32)pos.x, (int32)pos.y));

		Actor::Update();
	}
	virtual void Draw() override
	{
		if (state == U"default")
		{
			Actor::Draw();
		}
		else
		{
			mitame.drawFrame(2, Palette::Pink);
		}
		leg->drawFrame(3, Palette::Hotpink);

	}

	void OnCollsitionLeg()
	{
		//コンボ判定は敵側で行う
		//canhit
		if (canHit)
		{
			if (hp <= 0)
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

				if (RandomBool(0.5))
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
			hp -= 1;
		}
	}


private:

	float walkSpeed;//移動スピード
	float attackSpeed;//攻撃スピード（未使用）

	Duration damageDuration;//ダウン時間
	Timer damagedTimer;		//ダウンタイマー
	Vec2 damagedPos;		//攻撃受けた地点

	bool canHit = true;		//くらい判定　あり・なし　フラグ

	Duration invTimeDuration;		//無敵時間長さ
	Timer invTimeTimer;		//無敵時間タイマー

	String state = U"default";		//ステート

	void GenereteCoin()
	{
		Coin* item = new Coin(U"item",pos);
		*ptrCoinArray << item;
	}
	void GenereteItem()
	{
		ActorItem* item = new ActorItem(U"item", pos);
		*ptrItemArray << item;
	}
};

void CollectCoin()
{
	ptrPlayer->OnCollectCoin();
}

void OnGetItem(Item* item)
{
	ptrGameProperties->items << new ItemInstance(item);
}

void Main()
{
	comboCounter = new ComboCounter();

	//アイテム種類を定義
	itemList << new ItemAmulet();
	itemList << new ItemStatue();

	//ゲームの色んなデフォルト値を設定
	GameProperties gameProperties;

	gameProperties.fontDefault = Font{ 50 };
	gameProperties.coinGoal = 100000000;

	gameProperties.splashText << U"SplashA!";
	gameProperties.splashText << U"SplashBB!";

	ptrGameProperties = &gameProperties;

	Font font = Font{ 50 };

	Window::Resize(1600, 900);
	Scene::SetBackground(Palette::Forestgreen);

	Player* player = new Player(U"player");
	ptrPlayer = player;

	Array<Climber*> climbers;
	Timer climberGenerate;
	climberGenerate.set(3s);
	climberGenerate.start();
	RectF climberGenerateRect{ -500, 800 ,450 ,450 };//登山者が生成されるRect

	Array<Coin*> coins;
	ptrCoinArray = &coins;
	Array<ActorItem*> items;
	ptrItemArray = &items;

	//RectF climberGenerateRect{ 0, 0 ,450 ,450 };

	while (System::Update())
	{
		//マウススティック入力の更新
		inputDirector->Update();

		//コンボカウンター更新
		comboCounter->Update();

		//エフェクトの適用
		gameProperties.ApplyItem();

		//登山者生成
		if (climberGenerate.reachedZero())
		{
			for (int32 i = 0; i < gameProperties.climberRate; i++)
			{
				climbers << new Climber(U"climber", RandomVec2(climberGenerateRect));
			}
			climberGenerate.restart();
		}

		//登山者のアップデート
		climbers.each([](Climber* item) { item->Update(); });
		//コインのアップデート
		coins.each([](Coin* item) { item->Update(); });
		//アイテムのアップデート
		items.each([](ActorItem* item) { item->Update(); });

		//登山者の当たり判定
		//プレイヤーの位判定もここで行っている
		climbers.each([&player](Climber* item) {
				if (item->leg->intersects(*(player->leg)))
				{
					if (player->pos.x < item->pos.x)
					{
						//プレイヤーが登山者の左にいるときのみ登山者のくらい判定実行
						item->OnCollsitionLeg();
						//プレイヤーの方にも当たった時の処理を実行させる
						player->OnAttackHit();
					}
				}
				if (item->collision->intersects(*(player->collision)))
				{
					if (player->pos.x > item->pos.x)
					{
						//プレイヤーが登山者の右にいるときのみプレイヤーのくらい判定実行
						//歩きでは食らわない仕様：OnHit()内で判定してる
						player->OnHit();
					}

				}
			});
		//コインの当たり判定
		//コールバックでプレイヤーのコイン加算を行う
		coins.each([&player](Coin* item) {
				if (item->collision->intersects(*(player->collision)))
				{
					item->OnCollision(&player->pos,CollectCoin);
				}
			});

		//アイテムの当たり判定
		//コールバックでアイテム取得を行う
		items.each([&player](ActorItem* item) {
			if (item->collision->intersects(*(player->collision)))
			{
				item->OnCollision(&player->pos, OnGetItem);
			}
			});
		//削除済みフラグのある登山者を削除
		climbers.remove_if([](Climber* item) { return item->isDestroyed; });
		//削除済みフラグのあるコインを削除
		coins.remove_if([](Coin* item) { return item->isDestroyed; });

		//削除済みフラグのあるアイテム（アクター）を削除
		items.remove_if([](ActorItem* item) { return item->isDestroyed; });

		//プレイヤーのアップデート
		player->Update();

		//削除済みフラグのあるアイテム（取得済み）を削除
		gameProperties.items.remove_if([](ItemInstance* item) { return item->isDestroyed(); });

		//以下描画
		climbers.each([](Climber* item) { item->Draw(); });
		player->Draw();
		coins.each([](Coin* item) { item->Draw(); });
		items.each([](ActorItem* item) { item->Draw(); });

		inputDirector->Draw();

		comboCounter->Draw(font);

		gameProperties.Draw();
	}
}

