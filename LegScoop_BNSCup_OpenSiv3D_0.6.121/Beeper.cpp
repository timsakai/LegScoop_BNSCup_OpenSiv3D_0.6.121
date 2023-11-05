#include "stdafx.h"
#include "Beeper.h"

Beeps* Beeps::instance;

Audio Beeps::GetBeep(String _name)
{
	return instance->beeps[_name];
}

bool Beeps::AddBeep(String _name, Audio _audio)
{
	if (instance == nullptr)
	{
		instance = new Beeps();
	}
	bool success = false;
	if (instance->beeps.contains(_name))
	{
	}
	else
	{
		instance->beeps.emplace(_name, _audio);
		success = true;
	}
    return success;
}
