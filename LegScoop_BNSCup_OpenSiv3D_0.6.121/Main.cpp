# include <Siv3D.hpp> // Siv3D v0.6.12
#include "AnimatedTexture.h"
#include "InputDirector.h"
#include "ComboCounter.h"
#include "Item.h"
#include "GameProperties.h"


ComboCounter* comboCounter = nullptr;

InputDirector* inputDirector = new InputDirector();

GameProperties* ptrGameProperties = nullptr;

#include "Actor.h"

#include "ActorDeriveds.h"

Array<CritEffect*>* ptrEffectsArray;//エフェクト配列へのポインタ

Array<Coin*>* ptrCoinArray;//コイン（アクタ）インスタンス配列へのポインタ

#include "ItemDeriveds.h"

Array<Item*> itemList;//アイテム設計図のリスト

Array<ActorItem*>* ptrItemArray;//インスタンス配列へのポインタ

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
		Actor::Update();
	}
	virtual void Draw() override
	{

		Circle{ pos + Vec2{0,(state == U"damage") ? 0 : -100},40 }.draw(canHit ? Palette::Red : Palette::Pink);
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
		else if(state == U"attack")
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
		void select(AnimatedTextureInstance* _sel)
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
	};

	AnimTexSelect animSelect;

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
	virtual void Update() override
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

		animCurrent->GetTexture()->mirrored(false).draw(Arg::bottomCenter(pos));

	}

	void Attack()
	{
		if (state != U"attack")
		{
			state = U"attack";
			attackTimer.restart();
		}
	}

	void OnCollsitionLeg(Rect _leg)
	{
		//コンボ判定は敵側で行う
		//canhit
		if (canHit)
		{
			//ヘルス計算
			int32 damage = 1;
			if (RandomBool(Math::Map(Abs(leg->center().y - _leg.center().y),1,100,0,1)))
			{
				*ptrEffectsArray << new CritEffect(U"crit",leg->center());
				damage = -5;
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
		}
	}


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

	Texture backGround{ U"textures/Bkground.png" };

	Array<CritEffect*> effects;
	ptrEffectsArray = &effects;

	Player* player = new Player(U"player");
	ptrPlayer = player;

	//プレイヤーグラフィック定義
	AnimatedTexture* animPlayerStanding = new AnimatedTexture();
	animPlayerStanding->isLoop = true;
	animPlayerStanding->frames << TextureFrame{ Texture{ U"textures/Tex_Player_Stand.png"},1s };

	AnimatedTexture* animPlayerWalking = new AnimatedTexture();
	animPlayerWalking->isLoop = true;
	animPlayerWalking->frames << TextureFrame{ Texture{ U"textures/Tex_Player_Walk0.png"},0.3s };
	animPlayerWalking->frames << TextureFrame{ Texture{ U"textures/Tex_Player_Walk1.png"},0.3s };

	AnimatedTexture* animPlayerAttacking = new AnimatedTexture();
	animPlayerAttacking->isLoop = true;
	animPlayerAttacking->frames << TextureFrame{ Texture{ U"textures/Tex_Player_Attack.png"},1s };

	AnimatedTexture* animPlayerDamaged = new AnimatedTexture();
	animPlayerDamaged->isLoop = true;
	animPlayerDamaged->frames << TextureFrame{ Texture{ U"textures/Tex_Player_Damage.png"},1s };

	AnimatedTexture* animPlayerDown = new AnimatedTexture();
	animPlayerDown->isLoop = true;
	animPlayerDown->frames << TextureFrame{ Texture{ U"textures/Tex_Player_Down.png"},1s };

	player->texStanding = new AnimatedTextureInstance(animPlayerStanding);
	player->texWalking = new AnimatedTextureInstance(animPlayerWalking);
	player->texAttacking = new AnimatedTextureInstance(animPlayerAttacking);
	player->texDamaged = new AnimatedTextureInstance(animPlayerDamaged);
	player->texDown = new AnimatedTextureInstance(animPlayerDown);

	Array<Climber*> climbers;
	Timer climberGenerate;
	climberGenerate.set(3s);
	climberGenerate.start();


	//登山者グラフィック定義
	AnimatedTexture* animClimberStanding = new AnimatedTexture();
	animClimberStanding->isLoop = true;
	animClimberStanding->frames << TextureFrame{ Texture{ U"textures/Tex_Climber_Stand.png"},1s };

	AnimatedTexture* animClimberWalking = new AnimatedTexture();
	animClimberWalking->isLoop = true;
	animClimberWalking->frames << TextureFrame{ Texture{ U"textures/Tex_Climber_Walk0.png"},0.3s };
	animClimberWalking->frames << TextureFrame{ Texture{ U"textures/Tex_Climber_Walk1.png"},0.3s };

	AnimatedTexture* animClimberAttacking = new AnimatedTexture();
	animClimberAttacking->isLoop = true;
	animClimberAttacking->frames << TextureFrame{ Texture{ U"textures/Tex_Climber_Attack.png"},1s };

	AnimatedTexture* animClimberDamaged = new AnimatedTexture();
	animClimberDamaged->isLoop = true;
	animClimberDamaged->frames << TextureFrame{ Texture{ U"textures/Tex_Climber_Damage.png"},1s };

	AnimatedTexture* animClimberDown = new AnimatedTexture();
	animClimberDown->isLoop = true;
	animClimberDown->frames << TextureFrame{ Texture{ U"textures/Tex_Climber_Down.png"},1s };

	AnimatedTexture* animClimberDefeat = new AnimatedTexture();
	animClimberDefeat->isLoop = true;
	animClimberDefeat->frames << TextureFrame{ Texture{ U"textures/Tex_Climber_Defeat.png"},1s };

	AnimatedTexture* animClimberStandUp = new AnimatedTexture();
	animClimberStandUp->isLoop = true;
	animClimberStandUp->frames << TextureFrame{ Texture{ U"textures/Tex_Climber_StandUp.png"},1s };


	RectF climberGenerateRect{ -500, 800 ,450 ,450 };//登山者が生成されるRect

	Array<Coin*> coins;
	ptrCoinArray = &coins;
	Array<ActorItem*> items;
	ptrItemArray = &items;

	Array<Actor*> drawQueue;

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
				Climber* instance = new Climber(U"climber", RandomVec2(climberGenerateRect));
				climbers << instance;

				instance ->texStanding = new AnimatedTextureInstance(animClimberStanding);
				instance ->texWalking = new AnimatedTextureInstance(animClimberWalking);
				instance ->texAttacking = new AnimatedTextureInstance(animClimberAttacking);
				instance ->texDamaged = new AnimatedTextureInstance(animClimberDamaged);
				instance ->texDown = new AnimatedTextureInstance(animClimberDown);
				instance ->texDefeat = new AnimatedTextureInstance(animClimberDefeat);
				instance ->texStandUp = new AnimatedTextureInstance(animClimberStandUp);
			}
			climberGenerate.restart();
		}

		//登山者のアップデート
		climbers.each([](Climber* item) { item->Update(); });
		//コインのアップデート
		coins.each([](Coin* item) { item->Update(); });
		//アイテムのアップデート
		items.each([](ActorItem* item) { item->Update(); });
		//エフェクトのアップデート
		effects.each([](CritEffect* item) { item->Update(); });


		//登山者の当たり判定
		//プレイヤーの位判定もここで行っている
		climbers.each([&player](Climber* item) {
				if (item->leg->intersects(*(player->leg)))
				{
					if (player->pos.x < item->pos.x)
					{
						//プレイヤーが登山者の左にいるときのみ登山者のくらい判定実行
						item->OnCollsitionLeg(*(player->leg));
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
						//item->Attack();
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

		//削除済みフラグのあるエフェクトを削除
		effects.remove_if([](CritEffect* item) { return item->isDestroyed; });

		//プレイヤーのアップデート
		player->Update();

		//削除済みフラグのあるアイテム（取得済み）を削除
		gameProperties.items.remove_if([](ItemInstance* item) { return item->isDestroyed(); });

		//以下描画
		backGround.draw();

		drawQueue.clear();

		climbers.each([&drawQueue](Climber* item) { drawQueue << item; });
		drawQueue << player;

		drawQueue.sort_by([](const Actor* a, const Actor* b) { return (a->pos.y < b->pos.y); });

		int32 objcont = 0;
		for (const auto& item : drawQueue)
		{
			item->Draw();
			objcont++;
		}
		//climbers.each([](Climber* item) { item->Draw(); });
		//player->Draw();
		coins.each([](Coin* item) { item->Draw(); });
		items.each([](ActorItem* item) { item->Draw(); });

		effects.each([](CritEffect* item) { item->Draw(); });

		inputDirector->Draw();

		comboCounter->Draw(font);

		gameProperties.Draw();
	}
}

