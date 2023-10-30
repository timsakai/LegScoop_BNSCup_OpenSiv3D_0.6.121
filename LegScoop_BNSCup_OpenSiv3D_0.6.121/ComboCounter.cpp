#include "stdafx.h"
#include "ComboCounter.h"

ComboCounter::ComboCounter()
{
	comboDuration = 1.5s;
	comboTimer.set(comboDuration);
	isCommboning = false;
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
}

void ComboCounter::Draw(Font _font)
{
	if (isCommboning)
	{
		_font(combo).drawAt(1400, 200);
	}
}
