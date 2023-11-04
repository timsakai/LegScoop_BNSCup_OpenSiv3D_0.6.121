#include "Common.h"

extern ComboCounter* comboCounter;

extern InputDirector* inputDirector;

extern GameProperties* ptrGameProperties;

extern Array<CritEffect*>* ptrEffectsArray;//エフェクト配列へのポインタ

extern Array<Coin*>* ptrCoinArray;//コイン（アクタ）インスタンス配列へのポインタ

extern Array<Item*> itemList;//アイテム設計図のリスト

extern Array<ActorItem*>* ptrItemArray;//インスタンス配列へのポインタ

extern Player* ptrPlayer;

void CollectCoin()
{
	ptrPlayer->OnCollectCoin();
}

void OnGetItem(Item* item)
{
	ptrGameProperties->items << new ItemInstance(item);
}
