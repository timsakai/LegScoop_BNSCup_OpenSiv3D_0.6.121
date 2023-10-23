# include <Siv3D.hpp> // Siv3D v0.6.12

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
	}

	void Draw()
	{

		if (MouseL.pressed())
		{
			Circle{ PinchStart ,maxDistance }.drawFrame(2.0, Palette::Lightgrey);
			Circle{ Cursor::Pos(), 20 }.drawFrame(5.0, Palette::White);
		}
	}
private:
	Point PinchStart;

};

InputDirector* inputDirector = new InputDirector();

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

class Player : public Actor
{

public:
	float dir;
	float lastDir;
	Vec2 vel;
	Rect* leg;

	Player(String _name) : Actor(_name) {

		//歩行速度を初期化
		walkSpeed = 100;

		//キック攻撃速度を初期化
		attackSpeed = 1000;

		//はじき入力で判定される入力終了までの時間を初期化
		repelInputAcceptDuration = 0.1s;

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
				vel.x = 0;

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
					if (Abs(inputDirector->dir.x) >= 1)
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
	Vec2 vel;

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

		//速度を設定　角度と移動スピードから設定
		vel = OffsetCircular{ Vec2{0,0},walkSpeed,70_deg };
	}
	virtual void Update() override
	{
		//ステートが"defeated"の時は重力に従う
		//無条件で位置posに速度velを足す
		//画面外に出たら削除済みフラグisDestroyedを立てる
		if (state == U"defeated")
		{
			vel.y += 900 * Scene::DeltaTime();
		}

		pos += vel * Scene::DeltaTime();

		if (!collision->intersects(Rect{ -1000,0,4000,2000 }))
		{
			isDestroyed = true;
		}

		Actor::Update();
	}
	virtual void Draw() override
	{
		Actor::Draw();
	}

	void OnCollsitionLeg()
	{
		//ステートが"defeated"の時はくらい判定無し
		if (state != U"defeated")
		{

			state = U"defeated";
			vel = RandomVec2(RectF{ -1,-2,2,2 }) * 50;//速度を設定する
		}
	}

private:

	float walkSpeed;//移動スピード
	float attackSpeed;//攻撃スピード（未使用）

	String state = U"default";//ステート
};

void Main()
{
	Window::Resize(1600, 900);
	Scene::SetBackground(Palette::Forestgreen);

	Player* player = new Player(U"player");

	Array<Climber*> climbers;
	Timer climberGenerate;
	climberGenerate.set(1s);
	climberGenerate.start();
	RectF climberGenerateRect{ -500, 800 ,450 ,450 };//登山者が生成されるRect
	//RectF climberGenerateRect{ 0, 0 ,450 ,450 };

	while (System::Update())
	{
		//マウススティック入力の更新
		inputDirector->Update();

		//登山者生成
		if (climberGenerate.reachedZero())
		{
			climbers << new Climber(U"climber", RandomVec2( climberGenerateRect ));
			climberGenerate.restart();
		}

		//登山者のアップデート
		climbers.each([](Climber* item) { item->Update(); });

		//登山者の当たり判定
		//プレイヤーの位判定もここで行っている
		climbers.each([&player](Climber* item) {
				if (item->collision->intersects(*(player->leg)))
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

		//削除済みフラグのある登山者を削除
		climbers.remove_if([](Climber* item) { return item->isDestroyed; });

		//プレイヤーのアップデート
		player->Update();

		//以下描画
		climbers.each([](Climber* item) { item->Draw(); });
		player->Draw();
		inputDirector->Draw();
	}
}

