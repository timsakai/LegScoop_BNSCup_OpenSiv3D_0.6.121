#include "stdafx.h"
#include "InputDirector.h"

InputDirector::InputDirector()
{
	maxDistance = 100; //マウススティックの大きさ
	PinchStart = Point(0, 0);
}

void InputDirector::Update()
{
	dir = Vec2(0, 0);
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

void InputDirector::Draw()
{

	if (MouseL.pressed())
	{
		Circle{ PinchStart ,maxDistance }.drawFrame(2.0, Palette::Lightgrey);
		Circle{ Cursor::Pos(), 20 }.drawFrame(5.0, Palette::White);
	}
	if (const auto pro = ProController(0))
	{
		Circle{ Scene::Center(),maxDistance }.drawFrame(2.0, Palette::Lightgrey);
		Circle{ Scene::Center() + dir * maxDistance, 20 }.drawFrame(5.0, Palette::White);

	}
}
