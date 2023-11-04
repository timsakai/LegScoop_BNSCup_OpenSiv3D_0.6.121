#include "stdafx.h"
#include "GameProperties.h"

void GameProperties::ApplyItem()
{
	splashText.clear();
	coinPlus = 0;
	coinMultiply = 1;
	climberRate = 1;
	itemChance = 0.5;
	critChance = 1.0;
	for (int32 i = 0; i < items.size(); i++)
	{
		splashText << items.at(i)->item->splashText;
		items.at(i)->item->Effect();
	}
}

void GameProperties::Update()
{
	coinDisplay += Sign(coin - coinDisplay) * 1234;
	if (Abs(coin - coinDisplay) <= 1234) coinDisplay = coin;
}

void GameProperties::Draw() const
{
	fontDefault(U"￥", coinDisplay).drawBaseAt(40, 800, 750, Palette::White);
	fontDefault(U"/", coinGoal).drawBaseAt(20, 800, 800, Palette::White);
	Point SplashBase = Point{ 800, 700 };
	for (int32 i = 0; i < splashText.size(); i++)
	{
		fontDefault(splashText.at(i)).drawBaseAt(Math::Lerp(30, 25, Math::Sin(Scene::Time() * 3)), SplashBase + Point{ 0,-40 } *i, Palette::Yellow);
	}
}
