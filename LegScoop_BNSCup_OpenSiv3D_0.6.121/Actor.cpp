#include "stdafx.h"
#include "Actor.h"

Actor::Actor(String _name) {
	name = _name;
}

void Actor::Update()
{
	if (collision == nullptr) return;
	collision->setPos(Arg::bottomCenter((int32)pos.x, (int32)pos.y));	//コリジョン位置を更新
}

void Actor::Draw()
{
	if (collision == nullptr) return;
	collision->drawFrame(2.0, Palette::Greenyellow);	//コリジョンを表示
}
