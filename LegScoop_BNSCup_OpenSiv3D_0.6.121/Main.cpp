# include <Siv3D.hpp> // Siv3D v0.6.12
#include "Common.h"
#include "Beeper.h"

ComboCounter* comboCounter = nullptr;

InputDirector* inputDirector = new InputDirector();

GameProperties* ptrGameProperties = nullptr;

Array<CritEffect*>* ptrEffectsArray;//エフェクト配列へのポインタ

Array<Coin*>* ptrCoinArray;//コイン（アクタ）インスタンス配列へのポインタ

Array<Item*> itemList;//アイテム設計図のリスト

Array<ActorItem*>* ptrItemArray;//インスタンス配列へのポインタ

Player* ptrPlayer;




#pragma region Musicクラス
class Music
{
public:
	const Audio audio{ Audio::Stream, U"example/test.mp3", Loop::Yes };

	void update()
	{
		audio.setVolume(0.5).play();
	}
};
#pragma endregion





#pragma region シーン遷移

struct Data
{
	int32 data;
};

using App = SceneManager<String,Data>;

// タイトルシーン
class Title : public App::Scene
{
public:

	Title(const InitData& init)
		: IScene{ init }
		, title{ U"textures/Title.png" }
	{
		//Print << U"Title::Title()";
	}

	~Title()
	{
		//Print << U"Title::~Title()";
	}

	void update() override
	{
		// 左クリックで
		if (MouseL.down())
		{
			// ストーリーシーンに遷移
			changeScene(U"Story");
		}
	}

	void draw() const override
	{
		
		title.draw();
	}

private:

	Texture title;
};

// ストーリーシーン
class Story : public App::Scene
{
public:

	Story(const InitData& init)
		: IScene{ init }
		, story{ U"textures/Stories.png" }
	{
		// << U"Story::Story()";
	}

	~Story()
	{
		//Print << U"Story::~Story()";
	}

	void update() override
	{
		// 左クリックで
		if (MouseL.down())
		{
			// 説明シーンに遷移
			changeScene(U"Rule");
		}
	}

	void draw() const override
	{
		story.draw();
	}

private:

	Texture story;
};

// 説明シーン
class Rule : public App::Scene
{
public:

	Rule(const InitData& init)
		: IScene{ init }
		, rule{ U"textures/Controls.png" }
	{
		//Print << U"Rule::Rule()";
	}

	~Rule()
	{
		//Print << U"Rule::~Rule()";
	}

	void update() override
	{
		// 左クリックで
		if (MouseL.down())
		{
			// ゲームシーンに遷移
			changeScene(U"Game");
		}
	}

	void draw() const override
	{
		rule.draw();
	}

private:

	Texture rule;
};

// ゲームシーン
class Game : public App::Scene
{
public:
	Font font;

	GameProperties gameProperties;
	Texture backGround;
	Array<CritEffect*> effects;

	Player* player;

	AnimatedTexture* animPlayerDown;
	AnimatedTexture* animPlayerDamaged;
	AnimatedTexture* animPlayerAttacking;
	AnimatedTexture* animPlayerWalking;
	AnimatedTexture* animPlayerStanding;

	Array<Climber*> climbers;

	AnimatedTexture* animClimberAStanding;
	AnimatedTexture* animClimberAWalking;
	AnimatedTexture* animClimberAAttacking;
	AnimatedTexture* animClimberADamaged;
	AnimatedTexture* animClimberADown;
	AnimatedTexture* animClimberADefeat;
	AnimatedTexture* animClimberAStandUp;
	AnimatedTexture* animClimberBStanding;
	AnimatedTexture* animClimberBWalking;
	AnimatedTexture* animClimberBAttacking;
	AnimatedTexture* animClimberBDamaged;
	AnimatedTexture* animClimberBDown;
	AnimatedTexture* animClimberBDefeat;
	AnimatedTexture* animClimberBStandUp;

	Timer climberGenerate;
	RectF climberGenerateRect;

	Array<Coin*> coins;
	Array<ActorItem*> items;
	Array<Actor*> drawQueue;


	// タイマーの追加
	Timer timeUp;

	// Musicクラスの呼び出し
	Music music;



	Game(const InitData& init)
		: IScene{ init }
	{
		//Print << U"Game::Game()";

		comboCounter = new ComboCounter();

		//アイテム種類を定義
		itemList << new ItemAmulet();
		itemList << new ItemStatue();
		itemList << new ItemThron();
		itemList << new ItemLetter();
		itemList << new ItemScope();
		itemList << new ItemStatue();
		itemList << new ItemWheel();

		//ゲームの色んなデフォルト値を設定
		gameProperties = GameProperties();

		gameProperties.fontDefault = Font{ 50 };
		gameProperties.fontDefault.addFallback(gameProperties.fontEmoji);
		gameProperties.coinGoal = 1000000;

		ptrGameProperties = &gameProperties;



		// タイマーセットとスタート
		ptrGameProperties->remainingTime.set(180s);
		ptrGameProperties->remainingTime.start();
		//timeUp.set(180s);
		//timeUp.start();





		//gameProperties.splashText << U"SplashA!";
		//gameProperties.splashText << U"SplashBB!";


		font = Font{ 50 };

		Scene::SetBackground(Palette::Forestgreen);

		backGround = Texture{ U"textures/Bkground.png" };

		ptrEffectsArray = &effects;

		player = new Player(U"player");
		ptrPlayer = player;

		//プレイヤーグラフィック定義
		animPlayerStanding = new AnimatedTexture();
		animPlayerStanding->isLoop = true;
		animPlayerStanding->frames << TextureFrame{ Texture{ U"textures/Tex_Player_Stand.png"},1s };

		animPlayerWalking = new AnimatedTexture();
		animPlayerWalking->isLoop = true;
		animPlayerWalking->frames << TextureFrame{ Texture{ U"textures/Tex_Player_Walk0.png"},0.3s };
		animPlayerWalking->frames << TextureFrame{ Texture{ U"textures/Tex_Player_Walk1.png"},0.3s };

		animPlayerAttacking = new AnimatedTexture();
		animPlayerAttacking->isLoop = true;
		animPlayerAttacking->frames << TextureFrame{ Texture{ U"textures/Tex_Player_Attack.png"},1s };

		animPlayerDamaged = new AnimatedTexture();
		animPlayerDamaged->isLoop = true;
		animPlayerDamaged->frames << TextureFrame{ Texture{ U"textures/Tex_Player_Damage.png"},1s };

		animPlayerDown = new AnimatedTexture();
		animPlayerDown->isLoop = true;
		animPlayerDown->frames << TextureFrame{ Texture{ U"textures/Tex_Player_Down.png"},1s };

		player->texStanding = new AnimatedTextureInstance(animPlayerStanding);
		player->texWalking = new AnimatedTextureInstance(animPlayerWalking);
		player->texAttacking = new AnimatedTextureInstance(animPlayerAttacking);
		player->texDamaged = new AnimatedTextureInstance(animPlayerDamaged);
		player->texDown = new AnimatedTextureInstance(animPlayerDown);

		
		
		climberGenerate.set(3s);
		climberGenerate.start();


		//登山者グラフィック定義
		animClimberAStanding = new AnimatedTexture();
		animClimberAStanding->isLoop = true;
		animClimberAStanding->frames << TextureFrame{ Texture{ U"textures/Tex_ClimberA_Stand.png"},1s };

		animClimberAWalking = new AnimatedTexture();
		animClimberAWalking->isLoop = true;
		animClimberAWalking->frames << TextureFrame{ Texture{ U"textures/Tex_ClimberA_Walk0.png"},0.3s };
		animClimberAWalking->frames << TextureFrame{ Texture{ U"textures/Tex_ClimberA_Walk1.png"},0.3s };

		animClimberAAttacking = new AnimatedTexture();
		animClimberAAttacking->isLoop = true;
		animClimberAAttacking->frames << TextureFrame{ Texture{ U"textures/Tex_ClimberA_Attack.png"},1s };

		animClimberADamaged = new AnimatedTexture();
		animClimberADamaged->isLoop = true;
		animClimberADamaged->frames << TextureFrame{ Texture{ U"textures/Tex_ClimberA_Damage.png"},1s };

		animClimberADown = new AnimatedTexture();
		animClimberADown->isLoop = true;
		animClimberADown->frames << TextureFrame{ Texture{ U"textures/Tex_ClimberA_Down.png"},1s };

		animClimberADefeat = new AnimatedTexture();
		animClimberADefeat->isLoop = true;
		animClimberADefeat->frames << TextureFrame{ Texture{ U"textures/Tex_ClimberA_Defeat.png"},1s };

		animClimberAStandUp = new AnimatedTexture();
		animClimberAStandUp->isLoop = true;
		animClimberAStandUp->frames << TextureFrame{ Texture{ U"textures/Tex_ClimberA_StandUp.png"},1s };


		animClimberBStanding = new AnimatedTexture();
		animClimberBStanding->isLoop = true;
		animClimberBStanding->frames << TextureFrame{ Texture{ U"textures/Tex_ClimberB_Stand.png"},1s };

		animClimberBWalking = new AnimatedTexture();
		animClimberBWalking->isLoop = true;
		animClimberBWalking->frames << TextureFrame{ Texture{ U"textures/Tex_ClimberB_Walk0.png"},0.3s };
		animClimberBWalking->frames << TextureFrame{ Texture{ U"textures/Tex_ClimberB_Walk1.png"},0.3s };

		animClimberBAttacking = new AnimatedTexture();
		animClimberBAttacking->isLoop = true;
		animClimberBAttacking->frames << TextureFrame{ Texture{ U"textures/Tex_ClimberB_Attack.png"},1s };

		animClimberBDamaged = new AnimatedTexture();
		animClimberBDamaged->isLoop = true;
		animClimberBDamaged->frames << TextureFrame{ Texture{ U"textures/Tex_ClimberB_Damage.png"},1s };

		animClimberBDown = new AnimatedTexture();
		animClimberBDown->isLoop = true;
		animClimberBDown->frames << TextureFrame{ Texture{ U"textures/Tex_ClimberB_Down.png"},1s };

		animClimberBDefeat = new AnimatedTexture();
		animClimberBDefeat->isLoop = true;
		animClimberBDefeat->frames << TextureFrame{ Texture{ U"textures/Tex_ClimberB_Defeat.png"},1s };

		animClimberBStandUp = new AnimatedTexture();
		animClimberBStandUp->isLoop = true;
		animClimberBStandUp->frames << TextureFrame{ Texture{ U"textures/Tex_ClimberB_StandUp.png"},1s };


		climberGenerateRect = Rect{ -500, 800 ,450 ,450 };//登山者が生成されるRect

		ptrCoinArray = &coins;
		ptrItemArray = &items;

		
	}

	~Game()
	{
		//Print << U"Game::~Game()";
	}

	void update() override
	{



		// BGMの呼び出し
		music.update();

		

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

				if (RandomBool(0.4))
				{

					instance->texStanding = new AnimatedTextureInstance(animClimberAStanding);
					instance->texWalking = new AnimatedTextureInstance(animClimberAWalking);
					instance->texAttacking = new AnimatedTextureInstance(animClimberAAttacking);
					instance->texDamaged = new AnimatedTextureInstance(animClimberADamaged);
					instance->texDown = new AnimatedTextureInstance(animClimberADown);
					instance->texDefeat = new AnimatedTextureInstance(animClimberADefeat);
					instance->texStandUp = new AnimatedTextureInstance(animClimberAStandUp);
				}
				else
				{
					instance->texStanding = new AnimatedTextureInstance(animClimberBStanding);
					instance->texWalking = new AnimatedTextureInstance(animClimberBWalking);
					instance->texAttacking = new AnimatedTextureInstance(animClimberBAttacking);
					instance->texDamaged = new AnimatedTextureInstance(animClimberBDamaged);
					instance->texDown = new AnimatedTextureInstance(animClimberBDown);
					instance->texDefeat = new AnimatedTextureInstance(animClimberBDefeat);
					instance->texStandUp = new AnimatedTextureInstance(animClimberBStandUp);
				}

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
		climbers.each([this](Climber* item) {
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
		coins.each([this](Coin* item) {
			if (item->collision->intersects(*(player->collision)))
			{
				item->OnCollision(&player->pos, CollectCoin);
			}
			});

		//アイテムの当たり判定
		//コールバックでアイテム取得を行う
		items.each([this](ActorItem* item) {
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

		gameProperties.Update();

		drawQueue.clear();

		climbers.each([this](Climber* item) { drawQueue << item; });
		drawQueue << player;

		drawQueue.sort_by([](const Actor* a, const Actor* b) { return (a->pos.y < b->pos.y); });

		//以下描画
		
		// 左クリックで
		if (gameProperties.coin >= /*gameProperties.coinGoal*/1000000)
		{
			Beeps::GetBeep(U"Success").playOneShot();
			// ゲームクリアシーンに遷移
			changeScene(U"Clear");
		}
		// 右クリックで
		if (ptrGameProperties->remainingTime.reachedZero())
		{
			// ゲームオーバーシーンに遷移
			changeScene(U"Over");
		}
	}

	void draw() const override
	{
		//game.draw();
		backGround.draw();

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

private:

	Texture game;
};

// クリアシーン
class Clear : public App::Scene
{
public:

	Clear(const InitData& init)
		: IScene{ init }
		, clear{ U"textures/Clear.png" }
	{
		//Print << U"Clear::Clear()";
	}

	~Clear()
	{
		//Print << U"Clear::~Clear()";
	}

	void update() override
	{
		// 左クリックで
		if (MouseL.down())
		{
			// ゲームクリアシーンに遷移
			changeScene(U"Title");
		}
	}

	void draw() const override
	{
		clear.draw();
	}

private:

	Texture clear;
};

// オーバーシーン
class Over : public App::Scene
{
public:

	Over(const InitData& init)
		: IScene{ init }
		, over{ U"textures/Timeup.png" }
	{
		//Print << U"Over::Over()";
	}

	~Over()
	{
		//Print << U"Over::~Over()";
	}

	void update() override
	{
		// 左クリックで
		if (MouseL.down())
		{
			// ゲームクリアシーンに遷移
			changeScene(U"Title");
		}
	}

	void draw() const override
	{
		over.draw();
	}

private:

	Texture over;
};

#pragma endregion

void Main()
{


	Window::Resize(1600, 900);


	Beeps::AddBeep(U"Kick", Audio{ U"audios/KickA.mp3" });
	Beeps::AddBeep(U"KickCrit", Audio{ U"audios/KickA.mp3" });
	Beeps::AddBeep(U"Wind", Audio{ U"audios/Wind.mp3" });
	Beeps::AddBeep(U"Damage", Audio{ U"audios/Damage.mp3" });
	Beeps::AddBeep(U"Defeat", Audio{ U"audios/Defeat.mp3" });
	Beeps::AddBeep(U"Coin", Audio{ U"audios/Coin.mp3" });
	Beeps::AddBeep(U"Item", Audio{ U"audios/Item.mp3" });
	Beeps::AddBeep(U"Success", Audio{ U"audios/Success.mp3" });

#pragma region シーンマネージャー
	FontAsset::Register(U"TitleFont", 60, Typeface::Heavy);

	// シーンマネージャーを作成
	App manager;

	// タイトルシーン（名前は "Title"）を登録
	manager.add<Title>(U"Title");

	// ストーリーシーン（名前は "Story"）を登録
	manager.add<Story>(U"Story");

	// 説明シーン（名前は "Rule"）を登録
	manager.add<Rule>(U"Rule");

	// ゲームシーン（名前は "Game"）を登録
	manager.add<Game>(U"Game");

	// クリアシーン（名前は "Clear"）を登録
	manager.add<Clear>(U"Clear");

	// オーバーシーン（名前は "Story"）を登録
	manager.add<Over>(U"Over");

#pragma endregion

	while (System::Update())
	{
		
#pragma region マネージャー
		if (not manager.update())
		{
			break;
		}
#pragma endregion




	}
}


