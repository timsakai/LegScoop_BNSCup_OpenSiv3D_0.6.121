#include "stdafx.h"
#include "ComboCounter.h"

ComboCounter::ComboCounter()
{
	comboDuration = 1.5s;
	comboTimer.set(comboDuration);
	isCommboning = false;
	shakeTimer.set(0.2s);
}

void ComboCounter::Update()
{
	isCommboning = !comboTimer.reachedZero() && comboTimer.isRunning();
	if (!isCommboning)
	{
		combo = 0;
	}
}

void ComboCounter::Hit()
{
	comboTimer.restart();
	combo++;
	shakeTimer.restart();
}

void ComboCounter::Draw(Font _font)
{
	int32 shake = Sin(shakeTimer.sF() * 100) * shakeTimer.progress1_0() * 10;
	if (isCommboning)
	{
		_font(U"連撃").drawAt(1400 + shake * 0.5, 250);
		_font(combo).drawAt(100,1400 + shake, 200);
	}
}
