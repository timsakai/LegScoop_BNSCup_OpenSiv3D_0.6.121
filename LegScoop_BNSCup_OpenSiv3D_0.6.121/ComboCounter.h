#pragma once
//コンボカウントクラス
class ComboCounter
{
public:
	int32 combo = 0;
	Duration comboDuration;
	Timer comboTimer;
	bool isCommboning;
	ComboCounter();

	void Update();

	void Hit();

	void Draw(Font _font);
};
