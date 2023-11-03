# include <Siv3D.hpp> // Siv3D v0.6.12
#include "AnimatedTexture.h"
#include "InputDirector.h"
#include "ComboCounter.h"
#include "Item.h"
#include "GameProperties.h"
#include "Actor.h"
#include "ActorDeriveds.h"
#include "ItemDeriveds.h"


ComboCounter* comboCounter = nullptr;

InputDirector* inputDirector = new InputDirector();

GameProperties* ptrGameProperties = nullptr;

Array<CritEffect*>* ptrEffectsArray;//エフェクト配列へのポインタ

Array<Coin*>* ptrCoinArray;//コイン（アクタ）インスタンス配列へのポインタ

Array<Item*> itemList;//アイテム設計図のリスト

Array<ActorItem*>* ptrItemArray;//インスタンス配列へのポインタ

Player* ptrPlayer;

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
	itemList << new ItemThron();
	itemList << new ItemLetter();
	itemList << new ItemScope();
	itemList << new ItemStatue();
	itemList << new ItemWheel();

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
	AnimatedTexture* animClimberAStanding = new AnimatedTexture();
	animClimberAStanding->isLoop = true;
	animClimberAStanding->frames << TextureFrame{ Texture{ U"textures/Tex_ClimberA_Stand.png"},1s };

	AnimatedTexture* animClimberAWalking = new AnimatedTexture();
	animClimberAWalking->isLoop = true;
	animClimberAWalking->frames << TextureFrame{ Texture{ U"textures/Tex_ClimberA_Walk0.png"},0.3s };
	animClimberAWalking->frames << TextureFrame{ Texture{ U"textures/Tex_ClimberA_Walk1.png"},0.3s };

	AnimatedTexture* animClimberAAttacking = new AnimatedTexture();
	animClimberAAttacking->isLoop = true;
	animClimberAAttacking->frames << TextureFrame{ Texture{ U"textures/Tex_ClimberA_Attack.png"},1s };

	AnimatedTexture* animClimberADamaged = new AnimatedTexture();
	animClimberADamaged->isLoop = true;
	animClimberADamaged->frames << TextureFrame{ Texture{ U"textures/Tex_ClimberA_Damage.png"},1s };

	AnimatedTexture* animClimberADown = new AnimatedTexture();
	animClimberADown->isLoop = true;
	animClimberADown->frames << TextureFrame{ Texture{ U"textures/Tex_ClimberA_Down.png"},1s };

	AnimatedTexture* animClimberADefeat = new AnimatedTexture();
	animClimberADefeat->isLoop = true;
	animClimberADefeat->frames << TextureFrame{ Texture{ U"textures/Tex_ClimberA_Defeat.png"},1s };

	AnimatedTexture* animClimberAStandUp = new AnimatedTexture();
	animClimberAStandUp->isLoop = true;
	animClimberAStandUp->frames << TextureFrame{ Texture{ U"textures/Tex_ClimberA_StandUp.png"},1s };


	AnimatedTexture* animClimberBStanding = new AnimatedTexture();
	animClimberBStanding->isLoop = true;
	animClimberBStanding->frames << TextureFrame{ Texture{ U"textures/Tex_ClimberB_Stand.png"},1s };

	AnimatedTexture* animClimberBWalking = new AnimatedTexture();
	animClimberBWalking->isLoop = true;
	animClimberBWalking->frames << TextureFrame{ Texture{ U"textures/Tex_ClimberB_Walk0.png"},0.3s };
	animClimberBWalking->frames << TextureFrame{ Texture{ U"textures/Tex_ClimberB_Walk1.png"},0.3s };

	AnimatedTexture* animClimberBAttacking = new AnimatedTexture();
	animClimberBAttacking->isLoop = true;
	animClimberBAttacking->frames << TextureFrame{ Texture{ U"textures/Tex_ClimberB_Attack.png"},1s };

	AnimatedTexture* animClimberBDamaged = new AnimatedTexture();
	animClimberBDamaged->isLoop = true;
	animClimberBDamaged->frames << TextureFrame{ Texture{ U"textures/Tex_ClimberB_Damage.png"},1s };

	AnimatedTexture* animClimberBDown = new AnimatedTexture();
	animClimberBDown->isLoop = true;
	animClimberBDown->frames << TextureFrame{ Texture{ U"textures/Tex_ClimberB_Down.png"},1s };

	AnimatedTexture* animClimberBDefeat = new AnimatedTexture();
	animClimberBDefeat->isLoop = true;
	animClimberBDefeat->frames << TextureFrame{ Texture{ U"textures/Tex_ClimberB_Defeat.png"},1s };

	AnimatedTexture* animClimberBStandUp = new AnimatedTexture();
	animClimberBStandUp->isLoop = true;
	animClimberBStandUp->frames << TextureFrame{ Texture{ U"textures/Tex_ClimberB_StandUp.png"},1s };


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

